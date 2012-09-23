<?php

class SpacesOverridesBoxesMigration extends SpacesOverridesTableMigration {

  public function __construct() {
    $this->source_object = $this->dest_object = 'boxes';
    $this->dependencies = array('ModalImageNodeOS', 'SpacesOverridesContext', 'PageNodeOS', 'AnnouncementNodeOS', 'BiblioNodeOS'); //images have to be copied before modal boxes can migrate.
    parent::__construct();
  }

  public function query() {
    $query = parent::query();
    $query->condition('object_id', '', '!=');


    //$query->condition('value', '%os_boxes_node%', 'LIKE'); //testing the port function
    //$query->condition('value', '%os_boxes_modal_image%', 'LIKE'); //testing the port function
 //   $query->condition('value', '%manual%', 'LIKE');

    return $query;
  }


  public function prepareRow(&$row) {

    if (($value = @unserialize($row->value)) === FALSE) {
      if (strlen($row->value) > 65535) {
        //single image saved some images inline (ie img src='data:image:png....).  these were truncated in db and don't function
        parent::saveMessage(t('Box too big, source data truncated.  Ignoring.  id=@id', array('@id'=>$row->id), MigrationBase::MESSAGE_NOTICE));
      } else {
        parent::saveMessage(t('Can\'t unserialize box: id=@id, object_id=@object_id',
          array('@id' => $row->id, '@object_id' => $row->object_id)), MigrationBase::MESSAGE_WARNING);
      }
      return FALSE;
    }


    if ($box = $this->_update_box($value, $row->id)) {
      $value = $box;
    } else {
      parent::saveMessage(t('Could not migrate box: @key', array('@key'=>$value->plugin_key)), MigrationBase::MESSAGE_NOTICE);
      return FALSE;
    }

    if (!$this->_box_key_exists($box->plugin_key)) {
      parent::saveMessage(t('Box @key does not exist in destination.  Ignoring box.', array('@key'=>$box->plugin_key)), MigrationBase::MESSAGE_WARNING);
      return FALSE;
    }

//    $row->object_id = $object_id;
    $row->value = serialize($value);
    return parent::prepareRow($row);
  }

  /**
   * @see Migration::preImport()
   */
  public function preImport() {
    parent::preImport();

    $this->_copy_shield_files();
  }

  //after importing boxes, create 3 special boxes for what used to be custom themes.
  //don't worry about rollbacks.  spaces_overrides rollbacks delete entire table.
  public function postImport() {
    parent::postImport();


    //create new boxes
    $custom_boxes = array(
      '1' => array(
        'kshepsle_custom_box' => array(
          'region' => 'content_top',
          'body' => '<img src="/profiles/openscholar/themes/eloquent/flavors/green_modfront/shepsle-front.jpg">',
        ),
      ),
      '10057' => array(
        'agordon_custom_header_box' => array(
          'region' => 'header_second',
          'body' => '<img src="/profiles/openscholar/themes/eloquent/flavors/sand_modfront/gordonheader.gif">',
        ),
        'agordon_custom_menu_box' => array(
          'region' => 'menu_bar',
          'body' => '<ul class="os-menu"><li><a href="http://adgordon.net/biocv">Bio &amp; CV</a></li><li><a href="http://adgordon.net/publications">Publications</a></li><li><a href="http://adgordon.net/pages/teaching">Teaching</a></li><li><a href="http://adgordon.net/announcements" title="List of announcements in my site">News</a></li><li><a href="http://adgordon.net/links" title="List of links in my site">Links</a></li></ul>',
        ),
      ),
      '8' => array(
        'rbates_custom_box' => array(
          'region' => 'header_second',
          'body' => '<div class="os-rbates-map"></div>',
        ),
      ),
    );

    $new_boxes = array();
    foreach ($custom_boxes as $site => $boxes) {
      foreach ($boxes as $id => $box) {
        $new_box = array(
          'changed' => NULL,
          'delta' => $id,
          'title' => '',
          'description' => $id,
          'additional_classes' => '',
          'plugin_key' => 'os_boxes_html',
          'options' => array('text' => array('value'=>$box['body'], 'format'=>'filtered_html')),
          'info' => $id,
        );

        $new_boxes[] = array(
          'type' => 'og',
          'id' => $site,
          'object_type' => 'boxes',
          'object_id' => $id,
          'value' => serialize((object) $new_box),
        );
      }
    }

    $insert = db_insert('spaces_overrides')
      ->fields(array_keys($new_boxes[0]));
    foreach($new_boxes as $box) {
      $insert->values($box);
    }
    $insert->execute();



  }

  /**
   * @see Migration::postRollback()
   */
  public function postRollback() {
    parent::postRollback();

    $this->_remove_shield_files();
  }

  function _update_box($box, $id) {
    $BP = new BoxPorter($this);
    $box = $BP->_update_box($box, $id);

//     if (isset($box->region)) {
//       unset($box->region);
//     }

    return $box;
  }

  function _box_key_exists($key) {
    $BP = new BoxPorter($this);
    return $BP->_box_key_exists($key);
  }

  /*
   * @function _copy_shield_files
   *
   * Copies shields from d6's vsite_shields.  Sets up varibles so they're accessible in shields box.
   */
  function _copy_shield_files() {
    //path to legacy module
    $src = variable_get('os_migrate_files', '');
    $src .= '/' . parent::d6_variable_get('file_directory_path');
    $src .= '/vsite_shields/';

    //get files or bail out
//     if (($files = scandir($src)) === FALSE) {
//       parent::saveMessage(t('Could not read shields directory.  No shields migrated.'), MigrationBase::MESSAGE_NOTICE);
//       return FALSE;
//     }

    $dest = 'public://os_shields';
    file_prepare_directory($dest, FILE_CREATE_DIRECTORY);
    $shield_info = array();
    //$files = preg_grep('/\.png/', $files);

    //get a default
    $legacy_info = parent::d6_variable_get('vsite_shields_shield_info', FALSE);

    //add each file
    foreach ($legacy_info as $file => $info) {
      $filepath = $src . $file;
      $fh = fopen($filepath, 'r');
      if ($saved = file_save_data($fh, $dest.'/'.$file)) {
        $shield_info[$saved->filename]['hidden'] = $info['hidden'];
        $shield_info[$saved->filename]['default'] = $info['default'];
        $shield_info[$saved->filename]['fid'] = $saved->fid;
        if ($info['default']) {
          variable_set('os_shields_shield', $saved->filename);
        }
      }
      fclose($fh);
    }

    //and save the settings
    variable_set('os_shields_shield_info', $shield_info);
  }

  /**
   * @function _remove_shield_files
   *
   * Removes shields added by _copy_shield_files
   */
  function _remove_shield_files() {
    $shields = variable_get('os_shields_shield_info', array());
    foreach($shields as $fn => $info) {
      if ($file = file_load($info['fid'])) {
        file_delete($file, TRUE);
      }
    }

    variable_del('os_shields_shield_info');
    variable_del('os_shields_shield');
  }
}

/**
 * @class BoxPorter
 *
 * Box porting functions need to be avaialble to contexts, but shouldn't be part of the rest of migration.  Lets put them all here to contain them but make them accessible.
 *
 * @author sagotsky
 *
 */
class BoxPorter {

  public function __construct($migration) {
    $this->migration = $migration;
  }

  function _update_format($format = NULL) { //no format -> plain_text
    return (in_array($format, array(1,2,4,5,6))) ? 'filtered_html' : 'plain_text';
  }

  public function _update_block_plugin_key($key) {

    $box_map = array(
      'os_modal_tabbed' => 'os_boxes_tabs',
      'os_boxes_modal_sitelogo' => 'os_boxes_sitelogo', //needs pulling from orig vsite
      'os_boxes_modal_siteinfo' => 'os_boxes_siteinfo', //needs pulling from orig vsite
      'vsite_taxonomy_fbt' => 'os_taxonomy_fbt',
      'os_boxes_currtoc' => 'os_boxes_booktoc', //needs an option added
      'os_boxes_pub_typ_list' => 'os_boxes_pub_type',
    );


    //try mapping the key
    if (isset($box_map[$key])) {
      return $box_map[$key];
    }

    //check if we have a function for it
    if ($newkey = $this->_port_box($key, 0, TRUE)) {
      return $newkey;
    }

    //check if the key we're using exists or if we're ignoring it.
    if (module_exists('os_boxes')) {
      $boxes = os_boxes_get_boxes_plugins();
      if (isset($boxes[$key])) {
        return $key;
      } else {
        return FALSE;
      }
    } else {
      return $key;
    }

  }

  //os_sv_list_box uses function names for display types.  update them here.
  function _sv_list_box_style($style) {
    $style_map = array(
      'teasers' => 'display_teaser',
      'teaser' => 'display_teaser',
      'titles' => 'display_title',
      'title' => 'display_title',
      );

    return (isset($style_map[$style])) ? $style_map[$style] : 'display_teaser';
  }

  function _sv_list_box_sort($sort) {
    $sort_map = array(
      'a-z' => 'sort_alpha',
      'newest' => 'sort_newest',
      'oldest' => 'sort_oldest',
      );
    return (isset($sort_map[$sort])) ? $sort_map[$sort] : 'sort_newest';
  }


  public function _box_key_exists($plugin_key) {
    if (module_exists('os_boxes')) {
      $boxes = os_boxes_get_boxes_plugins();
      return isset($boxes[$plugin_key]);
    }
  }

  //refactored _update_block that also updates the box.
  public function _update_box($box, $id) {

    if (isset($box->region) && ($newregion = $this->migration->_update_region($box->region))) {
      $box->region = $newregion;
    }

    //matt says plugins should always have boxes as a module.
    if (isset($box->module)) {
      $box->module = $this->migration->_update_module($box->module);
    }

    /* three kinds of updates
     no change
    key changes
    key,box change
    box change
    delete
    */

    //first try box, box/key change.  if a function does this it should be complete and we can exit.
    $new = $this->_port_box($box, $id);
    if (isset($new)) {
      return $new; //set or false, it returns.
    }

    //key check.  either map a new key, or confirm that this key exists.
    if ($key = $this->_update_block_plugin_key($box->plugin_key)) {
      $box->plugin_key = $key;
      return $box;
    }

    return FALSE;
  }


  //wrapper function around the _port_box_$PLUGIN functions
  //key only is used for getting the key of the box without actually porting it.  seems easier than maintaining an array
  public function _port_box($box, $id, $key_only = FALSE) {
    if (is_string($box) && $key_only) {
      $box = (object) array('plugin_key' => $box); //make a box object for the purpose of fetching a new key.
    }

    $port = '_port_box_' . $box->plugin_key;
    if (method_exists($this, $port)) {
      return $this->$port($box, $id, $key_only);
    }

    return;
  }

  public function _port_box_os_boxes_bio($box, $id, $key_only) {
    if ($key_only) {
      return $id; //no change
    }

    if (isset($box->options['nid'])) {
      unset ($box->options['nid']);
    }

    return $box;
  }

  public function _port_box_os_boxes_modal_image($box, $id, $key_only) {
    if ($key_only) {
      return 'os_boxes_html';
    }

    //get fid of new entity from migrate's status tables
    $fid = db_select('migrate_map_modalimagenodeos', 'm')
      ->condition('sourceid1', $box->options['nid'])
      ->fields('m', array('destid1'))
      ->execute()
      ->fetchCol();
    $fid = current($fid);

    //get crop data from legacy db
    $image_info = $this->migration->d6_db_select('content_type_modal_image', 'ct')
      ->condition('nid', $box->options['nid'])
      ->fields('ct', array('field_modal_image_data', 'field_link_from_image_url', 'field_modal_image_size_value'))
      ->execute()
      ->fetchAssoc();

    if ($fid && ($file = file_load($fid))) {
      //migrate_map_modalimagenodeos.  sourceid1 = d6 nid.  destid1 = media id.

      $image_vars = array(
        'path' => $file->uri,
        //'width',
        //'height',
        'alt' => $box->description,
        'title' => $box->title,
        'attributes' => array(),
      );

      $img_html = theme_image($image_vars);
      if (isset($image_info['field_link_from_image_url'])) {
        $img_html = l($img_html, $image_info['field_link_from_image_url'], array('html' => TRUE));
      }

      //modal image should have a separate content type migration.  lets use that for moving the image.
      $box->plugin_key = 'os_boxes_html';
      $box->options = array('text' => array(
        'format' => 'filtered_html',
        'value' => $img_html,
      ));

      return $box;
    } else {
      //in most cases of a modal_image node that didn't import, there was no group owner.
      if ($fid === FALSE) {
        $fid = 'FALSE';
      }

      //OSMigration::saveMessage
      $this->migration->saveMessage(t('Could not port os_boxes_modal_image.  No file with fid @fid.  Check legacy node: @nid',
        array('@nid' => $box->options['nid'], '@fid' => $fid)), MigrationBase::MESSAGE_WARNING);
      return FALSE;
    }
  }

  // public function _port_box_os_boxes_modal_siteinfo($box, $key_only) {
  // $key = 'os_boxes_site_info';
  // if ($key_only) {
  // return $key;
  // }


  // //title
  // $query = parent::d6_db_select('node', 'n')
  // ->condition('type', 'vsite')
  // ->condition('nid', $nid)
  // ->fields('n', array('title'));

  // //address
  // $query->join('content_type_vsite', 'ct', 'n.nid = ct.nid');
  // $query->fields('ct', array('field_vsite_address_value', 'field_vsite_address_format'));

  // //description
  // $query->join('og', 'og', 'n.nid = og.nid');
  // $query->fields('og', array('og_description'));

  // if ($result = $query->execute()->fetchAssoc()) {
  // $box->plugin_key = $key;

  // $box->options = array(
  // 'site_title' => $result['title'],
  // 'descrip' => $result['og_descrioption'],
  // 'address1' => $result['field_vsite_address_value'],
  // );

  // return $box;
  // } else {
  // return FALSE;
  // }


  // }

  // public function _port_box_os_boxes_curr_toc($box, $key_only) {
  // $key = 'os_boxes_booktoc';
  // if ($key_only) {
  // return $key;
  // }

  // }

  //port html box
  public function _port_box_os_boxes_simple($box, $id, $key_only) {
    $key = 'os_boxes_html';
    if ($key_only) {
      return $key;
    }

    $body = $box->options['body'];
    $format = $this->_update_format($box->options['format']);

    unset($box->options['body'], $box->options['format']);

    $box->options = array('text' => array(
      'value' => $body,
      'format' => $format,
    ));
    $box->plugin_key = $key;

    return $box;
  }

  //attach a style option
  public function _port_box_os_boxes_manual_list($box, $id, $key_only) {
    if ($key_only) {
      return $id;
    }

    if (!isset($box->options['style'])) {
      $box->options['style'] = 'title';
    }

    return $box;
  }

  //wrap other shield funciton.  apparently shield boxes had differeing names
  public function _port_box_os_boxes_shields($box, $id, $key_only) {
    $key = 'os_shields_box';
    if ($key_only) {
      return $key;
    }

    return $this->_port_box_os_shields_box($box, $id, $key_only);
  }

  //'os_boxes_shields' => 'os_shields_box', (box holds the filename.  used to be in a var.)
  public function _port_box_os_shields_box($box, $id, $key_only) {
    $key = 'os_shields_box';
    if ($key_only) {
      return $key;
    }

    //this depends on variable having already been imported.
    $value = db_select('spaces_overrides', 'so')
      ->condition('object_type', 'variable')
      ->condition('object_id', 'os_shields_shield') //name has already been transformed...
      ->condition('id', $id)
      ->condition('type', 'og')
      ->fields('so', array('value'))
      ->execute()
      ->fetchCol();

    if (is_array($value)) {
      $value = (count($value)) ? $value[0] : '' ;
    }

    if (strlen($value) && ($shield_path = @unserialize($value))) {
      $shield_file = end(explode('/', $shield_path));
      $box->options['shield'] = $shield_file;
    } else {
      $box->options['shield'] = '';
    }
    $box->plugin_key = $key;

    return $box;
  }

  //recent docs list can convert to sv list
  public function _port_box_os_boxes_recent_docs($box, $id, $key_only) {
    $key = 'os_sv_list_box';
    if ($key_only) {
      return $key;
    }

    $box->options['filter'] = 'book';
    return $this->_port_box_os_boxes_sv_list($box, $id, $key_only);
  }

  //present list should be converted to sv list
  public function _port_box_os_boxes_present_list($box, $id, $key_only) {
    $key = 'os_sv_list_box';
    if ($key_only) {
      return $key;
    }

    $box->options['filter'] = 'presentation';
    return $this->_port_box_os_boxes_sv_list($box, $id, $key_only);
  }

  public function _port_box_os_boxes_sv_list($box, $id, $key_only) {
    $key = 'os_sv_list_box';
    if ($key_only) {
      return $key;
    }

    $box->plugin_key = $key;
    $ct = $box->options['filter'];
    if (strpos($ct, ':')) {
      list(,$ct) = explode(':', $ct);
    }

    $defaults = array(
      'content_type' =>     'all',
      'display' =>          'display_title',
      'more' =>             FALSE,
      'more_link' =>        '',
      'more_text' =>        '',
      'pager' =>            FALSE,
      'number_of_items' =>  6,
      'sort_by' =>          'sort_newest',
    );

    $options = $box->options + $defaults;

    $options['content_type'] = $ct;
    $options['display'] = $this->_sv_list_box_style($options['display']);
    $options['sort_by'] = $this->_sv_list_box_sort($options['sort_by']);
    $options['number_of_items'] = min(20, $options['number_of_items']); //new box only goes up to 20.
    $box->options = $options;

    return $box;
  }

  //twitter prefix is separated into own variable
  public function _port_box_os_boxes_twitterfeed($box, $id, $key_only) {
    if ($key_only) {
      return 'os_boxes_twitterfeed';
    }

    $prefix = $box->options['twitter_username'][0];
    if ($prefix == '@') {
      $box->options['twitter_type'] = 'user';
      $box->options['twitter_username'] = substr($box->options['twitter_username'], 1);
    } elseif ($prefix == '#') {
      $box->options['twitter_type'] = 'hashtag';
      $box->options['twitter_username'] = substr($box->options['twitter_username'], 1);
    } else {
      $box->options['twitter_type'] = 'search';
    }

    return $box;
  }

  public function _port_box_os_boxes_modal_sitelogo($box, $id, $key_only) {
    $key = 'os_boxes_site_logo';
    if ($key_only) {
      return $key;
    }

    if (($vsite = node_load($id)) == FALSE) {
      return FALSE;
    }

    if (isset($vsite->field_site_logo[LANGUAGE_NONE]) && $logo = current($vsite->field_site_logo[LANGUAGE_NONE])) {
      $box->options = array(
        'fid' => $logo['fid'],
        'x' => $logo['cropbox_x'],
        'y' => $logo['cropbox_y'],
        'height' => $logo['cropbox_height'],
        'width' => $logo['cropbox_width'],
      );
      $box->plugin_key = $key;

      return $box;
    } else {
      $this->migration->saveMessage(t('Could not migrate site logo. Image does not exist. Vsite nid: @nid', array('@nid' => $id)), MigrationBase::MESSAGE_WARNING);
      return FALSE;
    }
  }

  public function _port_box_os_boxes_gallery($box, $id, $key_only) {
    $key = 'os_boxes_slideshow';
    if ($key_only) {
      return $key;
    }

    $box->options += array(
      'controls' => 'none',
      'slide_counter' => 'none',
      'pager' => 'none',
      'effect' => 'fade',
      'style' => 'dark',
      'size' => 600,
    );

    $box->options['gallery_nid'] = $box->options['gallery'];
    unset($box->options['gallery']);

    $box->options['items_per_page'] = $box->options['number_of_items'];
    unset($box->options['number_of_items']);

    $box->plugin_key = $key;

    return $box;
  }

  //convert os_boxes_node to custom html.  102 in db
  public function _port_box_os_boxes_node($box, $id,  $key_only) {
    if ($key_only) {
      return 'os_boxes_html';
    }

    $body = '';

    $nid = $box->options['nid'];
    $teaser = $box->options['teaser'];
    $show_title = $box->options['show_title'];

    //some nids include the title.  (ie "this is my post[nid:1234]")
    if ((!(int)$nid) && strpos($nid, ':')) {
      list(,$nid) = explode(':', $nid);
      $nid = (int) $nid;
    }

    if (!$nid) {
      $this->migration->saveMessage(t('Could not port os_boxes_node.  Missing nid.'), MigrationBase::MESSAGE_WARNING);
      return FALSE; //can't port this without a nid.
    }

    //if nodes are already migrated, render new node.
    if ($node = node_load($nid)) {
      $build = node_view($node, $teaser);
      unset($build['#contextual_links']); //don't hardcode the admin links into the html box...
      $body = drupal_render($build);
      $node->format = 1; //make sure to use filtered html later
    } else {
      //if they haven't migrated, fetch the old node's body
      $query = $this->migration->d6_db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('n.nid', $nid);
      $query->join('node_revisions', 'nr', 'n.vid = nr.vid');
      $query->fields('nr', array('title', 'body', 'teaser', 'format'));
      $result = $query->execute();

      if ($result->rowCount()) {
        $node = $result->fetchObject();

        if ($show_title) {
          $body .= $node->title;
        }

        if ($teaser = 'body') {
          $body .= $node->body;
        } else {
          $body .= $node->teaser;
        }
      }
    }

    if ($body) {
      $box->plugin_key = 'os_boxes_html';
      $box->options = array('text'=>array(
        'value' => $body,
        'format' => $this->_update_format($node->format),
      ));

      return $box;
    } else {
      $this->migration->saveMessage(t('Could not port os_boxes_node.  No node with nid @nid.', array('@nid' => $nid)), MigrationBase::MESSAGE_WARNING);
      return FALSE;
    }
  }
}
