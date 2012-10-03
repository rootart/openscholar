<?php

/**
 * @class OSMigration
 *
 * Abstract class for all OpenScholar migration classes
 * Sets some defaults
 * Provides d6_db_select helper for getting d6 db for import
 */
abstract class OSMigration extends Migration {
  public function __construct() {
    // Always call the parent constructor first for basic setup
    $args = func_get_args();
    if (count($args)) {
      parent::__construct($args[0]);
    } else {
      parent::__construct();
    }

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Jon Sagotsky', 'jsagotsky@fas.harvard.edu', t('Migration Lead')),
    );

    // Individual mappings in a migration can be linked to a ticket or issue
    // in an external tracking system. Define the URL pattern here in the shared
    // class with ':id:' representing the position of the issue number, then add
    // ->issueNumber(1234) to a mapping.
    $this->issuePattern = 'https://redmine.hmdc.harvard.edu/issues/:id:';

    //$this->restrict_vsites = array('21094', '8921', '6879', '3633', '9015', '2', '1', '8', '10057');
  }

  /**
   * @function d6_db_select
   *
   * As db_select, but targets old d6 database
   * Sends users to admin page on a misconfigured db
   */
  static public function d6_db_select($target, $alias = NULL, array $options = array()) {
    static $os_migrate_database;
    if (!isset($os_migrate_database)) {
      $os_migrate_database =  variable_get('os_migrate_database', '');
    }

    try {
      return Database::getConnection('default', $os_migrate_database)
        ->select($target, $alias, $options);
    } catch (DatabaseConnectionNotDefinedException $e) {
      flood_register_event('os_migrate_bad_db');
      $msg = t('Unable to connect to legacy database %db  Please !link', array(
      				  '%db' => $os_migrate_database . '.',
      				  '!link'=>l('check settings','admin/config/openscholar/os_migrate')));
      drupal_set_message($msg, 'error');

      throw $e;
    }
  }

  public function d6_db_query ($query, array $args = array(), array $options = array()) {
    static $os_migrate_database;
    if (!isset($os_migrate_database)) {
      $os_migrate_database =  variable_get('os_migrate_database', '');
    }
    $options['target'] = $os_migrate_database;

    try {
      return Database::getConnection($options['target'], $os_migrate_database)->query($query, $args, $options);
    }  catch (DatabaseConnectionNotDefinedException $e) {
      flood_register_event('os_migrate_bad_db');
      $msg = t('Unable to connect to legacy database %db  Please !link', array(
          '%db' => $os_migrate_database . '.',
          '!link'=>l('check settings','admin/config/openscholar/os_migrate')));
      drupal_set_message($msg, 'error');

      throw $e;
    }
  }

  /**
   * @function d6_db_table_exists
   *
   * Check if table exists in legacy database.  Borrows code from db_table_exists.
   */
  function d6_db_table_exists($table) {
    static $os_migrate_database;
    if (!isset($os_migrate_database)) {
      $os_migrate_database =  variable_get('os_migrate_database', '');
    }

    try {
      return Database::getConnection('default', $os_migrate_database)->schema()->tableExists($table);
    } catch (DatabaseConnectionNotDefinedException $e) {
      flood_register_event('os_migrate_bad_db');
      $msg = t('Unable to connect to legacy database %db  Please !link', array(
          '%db' => $os_migrate_database . '.',
          '!link'=>l('check settings','admin/config/openscholar/os_migrate')));
      drupal_set_message($msg, 'error');

      throw $e;
    }
  }

  //list all content types used in d6
  function d6_list_content_types() {
    $query = self::d6_db_select('node', 'n')
      ->fields('n', array('type'))
      ->distinct();
    return $query->execute()->fetchCol();
  }
  /**
   * @function os_migrate_prepare_files
   *
   * Helper function for translating db results to array for file field.
   *
   * @param DatabaseStatementBase $result
   * 	Results from query on files table.  Requires filepath, description, list, and timestamp.
   *
   * @return array
   */
  // obsolete!  kept for reference
//   function os_migrate_prepare_files($result) {
//     $output = array();
//     foreach ($result as $row) {
//       $file = array(
//         'path' => variable_get('os_migrate_files', FALSE) . '/' . $row->filepath,
//         'description' => $row->description,
//         'display' => $row->list,
//         'timestamp' => $row->timestamp,
//         'title' => $row->title
//       );
//       $output[] = drupal_json_encode($file);
//     }
//     return $output;
//   }

  //update for previous function for v2.4 api.  instead of json, we use row->field:subfield.
  //we could provide db aliases and map to those, except that some fields (ie image galleries) have multiple values
  function os_migrate_prepare_files_v24($results, &$row, $field_name, $extra_fields = array()) {
    //or should we fetch row here?
    //$fields = array('alt', 'title') + $extra_fields; //assoc array for src/dst?

    //d6 col -> d7 field
    $fields = array('filepath' => '') + $extra_fields;

    foreach ($results as $result) {
          //also need a blank field for file itself.
      foreach ($fields as $col => $field) {
        $delta = (strlen($field)) ? "${field_name}_${field}" : $field_name;

        if (!isset($row->$delta)) {
          $row->$delta = array();
        }

   //     if (isset($result->$col)) {
          @array_push($row->$delta, $result->{$col}); //yes, pushing null when there's no result is correct.  it's a placeholder
     //   } else {
       //   array_push($row->$delta, NULL); //placeholder
       // }
      }
    }
  }

  //attaches results to row, minding the subfields (field => col)
  function os_migrate_prepare_files($files, &$row, $field_name, $subfields = array()) {
    //turn db statement into array of objects if that's what we're starting with.
    if (!is_array($files) && get_class($files) == 'DatabaseStatementBase') {
      $_files = array();
      while ($file_row = $files->fetchObject()) {
        $_files[] = $file_row;
      }
      $files = $_files;
    }

    //make subfields associative if it isn't.
    if (array_keys($subfields) === range(0, count($subfields)-1)) {
      $subfields = array_combine($subfields, $subfields);
    }

    //don't forget the field itself
    $subfields[''] = 'filepath';

    //create field if necessary, add results to it.
    foreach ($files as $file) {
      foreach ($subfields as $field => $column) {
        $delta = (strlen($field)) ? "${field_name}_${field}" : $field_name;

        if (!isset($row->$delta)) {
          $row->$delta = array();
        }
        //null results SHOULD be pushed.  they're placeholders.
        @array_push($row->$delta, $file->$column);
      }
    }

  }

  function get_file_by_fid($fid) {
    $result = $this->d6_db_select('files', 'f')
      ->fields('f')
      ->condition('fid', $fid, '=')
      ->execute();
    return $result->fetchObject();
  }

  /**
   * @function subfields
   *
   * Shortcut for getting an array of field:sub1, field:sub2, etc without typing field over and over.
   *
   * @param string $field
   *   Name of field
   * @param array $subfields
   *   List of subfields
   * @param string $glue
   *   Specify another string to use between field subfield.  Default=':'.
   * @return array
   */
  function subfields($field, array $subfields, $glue = ':') {
    $callback = create_function('$str', 'return "' . $field . $glue . '" . $str;');
    return array_map($callback, $subfields);
  }

  /**
   * @function os_migrate_node_get_files_query
   *
   *
   * @param int $vid - revision of node
   * @return $query - legacy db query to get list of files
   */
  function os_migrate_node_get_files_query($vid) {
    $query = self::d6_db_select('upload', 'u')
      ->fields('u', array('fid', 'vid', 'description', 'list'))
      ->condition('u.vid', $vid, '=');
    $query->join('files', 'f', 'u.fid = f.fid');
    $query->addField('f', 'filepath');
    $query->addField('f', 'timestamp');
    $query->orderBy('u.fid', 'ASC');
    return $query;
  }

  /*
   * Get variable from d6.  Cache var.
   */
  function d6_variable_get($var, $default=NULL) {
    static $variable_cache;

    if (!isset($variable_cache)) {
      $variable_cache = array();
    }

    if (!isset($variable_cache[$var])) {
      $query = self::d6_db_select('variable', 'v')
        ->fields('v', array('value'))
        ->condition('v.name', $var, '=');
      $value = unserialize($query->execute()->fetchField());
      $variable_cache[$var] = (isset($value)) ? $value : $default;
    }

    return $variable_cache[$var];
  }

  /*
   * @function is_domain
   *
   * Checks if we're using a certain domain.  Uses purl_base_domain by default, but
   * this can be overridden with a dev var.
   *
   * Function exists for edge and special cases that may vary between installations.
   */
  function is_domain($domain) {
    if ($domain == variable_get('os_migrate_use_domain')) {
      return TRUE;
    }

    if ($domain == self::d6_variable_get('purl_base_domain')) {
      return TRUE;
    }

    return FALSE;
  }

  /*
   * Check that a file exists before letting migrate try to copy it.
   * Inline images and files have been looping when a file isn't available.
   * This helper function processes the path in the same way before trying to
   * copy so we can bail out if the file is unavailable
   */
  function legacy_file_exists($path, $log_error=TRUE) {
//  	$path = trim(parse_url($path, PHP_URL_PATH), '/');
    $path = str_replace(' ', '%20', $path);

    //try file first
    if (file_exists($path)) {
    	return TRUE;
    }

    //now try url
    $headers = @get_headers($path);
    if ($headers && ($headers[0] != 'HTTP/1.0 404 Not Found')) {
    	return TRUE;
    }

    if ($log_error) {
  	  self::saveMessage(t('Could not copy inline file %uri.  Ignoring it.', array('%uri'=>$path)), MigrationBase::MESSAGE_WARNING);
    }
    return FALSE;
  }



   /*************************
    * inline file processing
    *****************************/

    /**
     * @function os_migrate_list_inline_files
     *
     * Gets all the inline files associated with a nid.  Also checks if node bodies
     * contain inline objects that may not be in inline table
     **/
    function os_migrate_list_inline_files($nid) {
      $files = array();

      //map of tables/fields which contain references to inline media
      $table_fields = array(
        'content_field_os_inline_files' => 'field_os_inline_files_fid',
        'content_field_os_inline_image' => 'field_os_inline_image_fid',
        'content_field_os_inline_oembed' => 'field_os_inline_oembed_url',
      );

      //query each table individually.  one big query consumes too much memory.
      foreach($table_fields as $table => $field) {
        $query = $this->d6_db_select('node', 'n')
          ->condition('n.nid', $nid, '=');
        $query->join($table, 't', 'n.nid = t.nid');
        $query->fields('t', array($field));
        $query->isNotNull($field);

        foreach ($query->execute() as $row) {
          $files[] = array_filter((array)$row);
        }
      }

      //and check the node bodies (done separately since condition is more complicated

      $query = $this->d6_db_select('node', 'n')
        ->condition('n.nid', $nid, '=');
      $query->join('node_revisions', 'nr', 'n.nid = nr.nid');
      $query->fields('nr', array('body'));
      $query->condition(db_or()
        ->condition('nr.body', '%<iframe%', 'LIKE')
        ->condition('nr.body', '%<object%', 'LIKE')
      );

      foreach ($query->execute() as $row) {
        $arr = array_filter((array)$row);
        if (!count($arr)) { $arr['body'] = TRUE; }
        $files[] = $arr;
      }

    return $files;
  }

  /**
   * @function os_migrate_update_body_files(&$row, $fid)
   *
   * Given a node's row and the fid of an inline file in that node, update the body text to
   * refer to the new file entity
   **/
  function os_migrate_update_body_files(&$row, $fid) {
    $query = db_select('migrate_map_fileinlineos', 'm')
      ->fields('m', array('destid1'))
      ->condition('sourceid1', $fid, '=')
      ->isNotNull(('destid1'));
    $result = $query->execute();
    $file = $result->fetchAssoc();
    if (!$file) {
      return;
    }

    $entity = file_load($file['destid1']);
    $uri = $this->_uri_trim($entity->uri);

    $dom = filter_dom_load($row->body);
    $links = $dom->getElementsByTagName('a');
    $update = FALSE;
    for ($i=0,$len=$links->length; $i<$len; $i++) {
      $link = $links->item($i);
      if ($link && strpos($link->getAttribute('href'), $uri)) {
        $update = TRUE;
        $tag = $this->_os_migrate_inline_tag('file', $entity->fid, $link);
        $text = $dom->createTextNode($tag);
        $link->parentNode->replaceChild($text, $link);
      }
    }

    if ($update) {
      $row->body = $this->_os_migrate_update_body_clean($dom->saveHTML());
    }
  }

  /**
   * @function os_migrate_update_body_url(&$row, $url)
   *
   * Given a node's row and the url of an inline link in that node, update the body text to
   * refer to the new file entity
   **/
  function os_migrate_update_body_url(&$row, $url) {
    $query = db_select('migrate_map_embedinlineos', 'm')
      ->fields('m', array('destid1'))
      ->condition('sourceid1', $row->vid, '=');

    $entity_id = $query->execute()->fetchAssoc();
    if (!$entity_id) {
      parent::saveMessage(t('Could not update url in node body.  Missing file entity. @nid', array('@nid'=>$row->nid)), MigrationBase::MESSAGE_NOTICE);
      return FALSE;
    }

    $entity = file_load($entity_id['destid1']);

    $dom = filter_dom_load($row->body);
    //check all a tags.  we'll have to replace parent though since its a space for the oembed
    $anchors = $dom->getElementsByTagName('a');
    $update = FALSE;
    for ($i=0, $len=$anchors->length; $i<$len; $i++) {
      $a = $anchors->item($i);
      if (isset($a) && ($a->getAttribute('href') == $url)) {
        $tag = $this->_os_migrate_inline_tag('url', $entity->fid, $a);
        $text = $dom->createTextNode($tag);
        $span = $a->parentNode;
        $span->parentNode->replaceChild($text, $span);

        $update = TRUE;
      }
    }

    if ($update) {
      $row->body = $this->_os_migrate_update_body_clean($dom->saveHTML());
    }
  }

  /**
   * @function os_migrate_update_body_body(&$row, $fid)
   *
   * Given a node's row and the fid of a file extracted from the body of the node, update the body text to
   * refer to the new file entity
   **/
  function os_migrate_update_body_body(&$row, $fid) {
    $query = db_select('os_migrate_body_inline_objects', 'o')
      ->fields('o', array('html'))
      ->condition('vid', $row->vid, '=');
    $query->join('migrate_map_htmlinlineos', 'm', 'o.id = m.sourceid1');
    $query->fields('m', array('destid1'));
    $query->isNotNull('m.destid1');

    $data = $query->execute()->fetchAssoc();
    $update = FALSE;

    //lets try iframes first
    $target_dom = filter_dom_load($data['html']);
    $target_iframes = $target_dom->getElementsByTagName('iframe');
    if ($target_iframes->length) {
      $target_src = $target_iframes->item(0)->getAttribute('src');

      $dom = filter_dom_load($row->body);
      $iframes = $dom->getElementsByTagName('iframe');
      for ($i=0,$len=$iframes->length; $i<$len; $i++) {
        $iframe = $iframes->item($i);
        if ($iframe && ($src = $iframe->getAttribute('src'))) {
          if ($src == $target_src) {
            $update = TRUE;
            $tag = $this->_os_migrate_inline_tag('iframe', $data['destid1'], $iframe);
            $text = $dom->createTextNode($tag);
            $iframe->parentNode->replaceChild($text, $iframe);
          }
        }
      }
    }

    //and objects.  some have data.  others have children embeds.
    $target_objects = $target_dom->getElementsByTagName('object');
    if ($target_objects->length) {
      $target_object = $target_objects->item(0);
      $target_data = $target_object->getAttribute('data');
      $target_embed = $target_object->getElementsByTagName('embed');
      if ($target_embed->length) {
        $target_embed_src = $target_embed->item(0)->getAttribute('src');
      }

      $dom = filter_dom_load($row->body);
      $objects = $dom->getElementsByTagName('object');
      for ($i=0,$len=$objects->length; $i<$len; $i++) {
        $object = $objects->item($i);
        if ($object && ($src = $object->getAttribute('data'))) {
          //data objects - currently untested.
          if ($target_data == $src) {
            $update = TRUE;
            $tag = $this->_os_migrate_inline_tag('object-data', $data['destid1'], $object);
            $text = $dom->createTextNode($tag);
            $object->parentNode->replaceChild($text, $object);

          } elseif ($object && ($embed = $object->getElementsByTagName('embed'))) {
            //embed objects
            if ($target_embed_src == $embed->item(0)->getAttribute('src')) {
              $update = TRUE;
              //can't convince wysiwyg to take these strings.  lets ignore that for now and update tag later.
              $tag = $this->_os_migrate_inline_tag('object-embed', $data['destid1'], $object);
              $text = $dom->createTextNode($tag);
              $object->parentNode->replaceChild($text, $object);
            }
          }
        }
      }
    }

    if ($update) {
      $row->body = $this->_os_migrate_update_body_clean($dom->saveHTML());
    }

    return;
  }

  /**
   * @function os_migrate_update_body_image(&$row, $fid)
   *
   * Given a node's row and the fid of an inline image in that node, update the body text to
   * refer to the new file entity
   **/
  function os_migrate_update_body_image(&$row, $fid) {
    $query = db_select('migrate_map_imageinlineos', 'm')
      ->fields('m', array('destid1'))
      ->condition('sourceid1', $fid, '=')
      ->isNotNull(('destid1'));
    $entity_id = $query->execute()->fetchAssoc();
    if (! ($entity_id && $entity = file_load($entity_id['destid1']))) {
      self::saveMessage(t('Migration error on source file %fid.  Could not find file in migration tables.  Continuing without file.  Check Inline migrations.', array('%fid'=>$fid)), MigrationBase::MESSAGE_WARNING);
      return;
    }

    //replace this with generic function.  does the one I wrote make sense?  wtf is this one doing?
    //were we stripping just the purl for the site?
    $uri = self::_uri_trim($entity->uri);/*
    $uri = str_replace('public://', '', $entity->uri);
    if (strpos($uri, '/') !== FALSE) {
      $a = explode('/', $uri);
      $a = array_slice($a, 2);
      $uri = implode('/', $a);
    }*/

    $dom = filter_dom_load($row->body);
    $images = $dom->getElementsByTagName('img');
    $update = FALSE;
    for ($i=0, $len=$images->length; $i < $len; $i++) {
      $img = $images->item($i);
      if (isset($img) && strpos($img->getAttribute('src'), $uri)) {
        $tag = $this->_os_migrate_inline_tag('image', $entity->fid, $img);
        $text = $dom->createTextNode($tag);
        $img->parentNode->replaceChild($text, $img);
        $update = TRUE;
      }
    }

    if ($update) {
      $row->body = $this->_os_migrate_update_body_clean($dom->saveHTML());
    }

  }

  /**
   * @function os_migrate_inline_update
   *
   * Check node row for inline embeds.  If any exist update body to use new file entities.
   **/
  function os_migrate_inline_update(&$current_row) {
    $files = $this->os_migrate_list_inline_files($current_row->nid);
    if (!count($files)) {
      return;
    }

    $file_funcs = array(
      'field_os_inline_image_fid' => 'image',
      'field_os_inline_files_fid' => 'files',
    	'field_os_inline_oembed_url' => 'url',
    	'body' => 'body',
    );

    foreach ($files as $file) {
      foreach ($file as $src => $fid) {
        $func = 'os_migrate_update_body_' . $file_funcs[$src];
        $this->$func($current_row, $fid);
      }
    }
    return;
  }

  //prepare the inline tag
  //generic for now.
  function _os_migrate_inline_tag($type, $fid, &$dom_object) {
    $tag = array(
      'type' => 'media',
      'view_mode' => 'media_original',
      'fid' => $fid,
      'attributes' => array(
        'class' => 'media-image',
        'typeof' => 'foaf:Image',
      ),
    );

    $sizes = array(
      'imagecache-inline_larger' => 'media_large',
      'imagecache-inline_default' => 'media_original',
      '' => 'media_large',
    );

    $attributes = array();

    switch ($type) {
      case 'image':
        //images may have titles but there's no way to enter a title in media
        $class = $dom_object->getAttribute('class');
        if (isset($sizes[$class])) {
          $tag['view_mode'] = $sizes[$class];
        }
        $attributes = array('width'=>'width', 'height'=>'height');
      break;

      case 'file':
      break;

      case 'url':
      break;

      case 'iframe':
        $attributes = array('width'=>'width', 'height'=>'height');
        //style attr?  keep or ditch
      break;
    }

    $tag['attributes'] += $this->_os_migrate_get_attributes($attributes, $dom_object);
    return ' [[' . drupal_json_encode($tag) . ']] ';
  }

  /**
   * get attributes from dom element
   * attr array maps old (d6) to new d7 attribute, just in case things change due to new schema
   **/
  function _os_migrate_get_attributes(array $attr, DOMElement $dom) {
    $return = array();
    foreach ($attr as  $old => $new) {
      if ($value = $dom->getAttribute($old)) {
        $return[$new] = $value;
      }
    }
    return $return;
  }

  //body saved by dom is full document.  strip doctype, html, body tags.
  function _os_migrate_update_body_clean($body) {
    $body = preg_replace('/<!DOCTYPE.*?>/', '', $body); //remove doctype
    $body = preg_replace('/.*\<body\>/m', '', $body); //remove html, body
    $body = preg_replace('/<\/body>.*/m', '', $body); //remove closing tags
    return $body;
  }

  //remove public:// or whatever :// appears before uri
  function _uri_trim($uri) {
    $pattern = '/.*?:\/\//';
    if (preg_match($pattern, $uri)) {
      $uri = preg_replace($pattern, '', $uri);
    }
    return $uri;
  }

  //file entities have no field for og_group_ref.  update file_usage
  //to let drupal know which node they're attached to
  function _os_migrate_inline_files_update_file_usage($entity, $row) {
    if ($entity->fid && $row->nid) {
      file_usage_add($entity, 'os_wysiwyg', 'node', $row->nid);
    } else {
      return FALSE; //does this ever happen?  nid==0
    }
  }
}


