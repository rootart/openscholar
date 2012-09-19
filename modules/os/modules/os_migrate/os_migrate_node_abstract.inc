<?php

/**
 * @file
 *
 * Abstract migration class for nodes
 **/


/**
 * Abstract node migration class
 *
 * Defines mappings for node table.  Let each type implement this and dscribe the specifics
 */
abstract class AbstractNodeOSMigration extends OSMigration {

  public function __construct($destination_bundle, $source_content_type, $create = TRUE) {
    $this->bundle = $destination_bundle;


    parent::__construct(MigrateGroup::getInstance('node', array('inline', 'users')));
    //when extending this class, don't forget to update info file with new filename!

    // if desired, supply node with defaults.
    if ($create) {
      $this->source = $this->source($source_content_type); //condtion type?
      $this->destination = $this->destination($destination_bundle);
      $this->map = $this->map($source_content_type);
      $this->dependencies = array('UsersOS', 'EmbedInlineOS', 'ImageInlineOS', 'FileInlineOS', 'HTMLInlineOS', 'VsiteNodeOS'); //og when that's done

      /* mappings */
      $this->addSimpleMappings(array('created','changed','status','promote','sticky','comment',
                                     'title', 'tnid', 'uid', 'nid'));

      $this->addFieldMapping('language', 'language')->defaultValue('und');
      $this->addFieldMapping('is_new')->defaultValue(TRUE);
      $this->addFieldMapping('revision_uid', 'nr_uid');

      $this->addFieldMapping('body', 'body');
      $this->addFieldMapping('body:format', 'format');
      $this->addFieldMapping('body:language', 'language')->defaultValue('und');
      $this->addFieldMapping('body:summary', 'teaser');

      $this->addFieldMapping('log', 'log'); //from nr
      $this->addFieldMapping('revision')->defaultValue(TRUE); //create a new revision.  vid is automatically migrated
      $this->addFieldMapping('path', 'path');
      $this->addFieldMapping('pathauto')->defaultValue(FALSE); //do not create new paths.  paths already exist.

      $this->_map_file_field();

      //dev setting: force nodes to be in a set group
      $force_og = variable_get('os_migrate_force_og', 0);
      if (!$force_og) {
        $this->addFieldMapping('og_group_ref', 'group_nid');
      } else {
        $this->addFieldMapping('og_group_ref')
          ->defaultValue($force_og);
        $this->addUnmigratedSources(array('group_nid'), t('DNM'));
      }

//      parent::addMappingsCCK(); //map additional cck fields

      /** migrate <= 2.3 files are deprecated! **
      $associated_files_args = MigrateFileFieldHandler::arguments(NULL, 'file_copy', FILE_EXISTS_REPLACE);
      $this->addFieldMapping('field_file_upload', 'file_upload')->arguments($associated_files_args);
      **/
      // MigrateFileURI is default file, no need to specify
      //$this->addFieldMapping('field_file_upload')

      /* Unmapped source fields.  Mark them as Do Not Map, so the UI knows they're intentionally ignored */
      $this->addUnmigratedSources(array('moderate', 'translate', 'type', 'format_name', 'vid'), t('DNM'));
//      $this->addUnmigratedSources(array('oe_vid', 'oe_nid', 'delta', 'field_os_inline_oembed_url', 'field_os_inline_oembed_title', 'field_os_inline_oembed_attributes') ,t('DNM'));
      //moderate appears to have been removed from node.
      //type.  we're doing nodes one type at a time.  type should be handled there.
      //translate unknown

    }
    //else - user is responsible for doing parent:query() and setting up source, destination, and map
  }

  /**
   * several helper functions for setting up child nodes.
   * it's not clear to me yet what level of abstraction I'll be working at
   * so for now I'm leaivng it up to the node that implements this class
   * to use the provided options or come up with their own.
   */

  public function query($content_type) {
    $query = parent::d6_db_select('node', 'n')
                    ->fields('n', $this->getNodeFields())
                    ->condition('n.type', $content_type, '=');

    //nodes to test files with:
    //$query->condition('n.nid', '20301', '='); //prod node with uploads
    //$query->condition('n.nid', '6811', '='); //prod node with inline files
    //$query->condition('n.nid', '13701', '='); //prod node with multiple images

    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');
    $query->fields('nr', array('body', 'uid', 'log', 'teaser'));

    $query->join('filter_formats', 'ff', 'nr.format = ff.format');
    $query->addField('ff', 'name', 'format_name');
    $query->orderBy('n.changed');

    $query->join('og_ancestry', 'og', 'n.nid = og.nid');
    $query->fields('og', array('group_nid'));
    $query->condition('og.group_nid', '0', '!=');

    $table = 'content_type_' . $content_type;
    if (parent::d6_db_table_exists($table)) {
      $query->leftjoin($table, 'ct', 'n.vid = ct.vid');
			$query->fields('ct');
    }


    if (isset($this->restrict_vsites)) {
      $query->condition('og.group_nid', $this->restrict_vsites, 'IN'); //dancabral, jlepore, jsagotsky, gary
    }


    return $query;
  }

  private function getNodeFields() {
    return array('nid', 'vid', 'type','language','title','uid','status','created', 'changed','comment','promote','moderate','sticky','tnid','translate');
  }

  public function sourceFields($content_type) {
    $source_fields =  array(
    	'nid'   => t('Node ID'),
      'path'  => t('URL Path Alias'),
      'format' => t('Text format.  Usually filtered html.'),
//      'file_upload' => t('File attachments'), //this may have to move to individual classes
      //'nr.body'	=> t('Node\'s Content')
    );

    //get columns front content_type_$content_type
    $table = 'content_type_' . $content_type;
    if (parent::d6_db_table_exists($table)) {
	    $result = parent::d6_db_query('DESCRIBE '. $table);
	    while ($row = $result->fetchAssoc()) {
	      $field = $row['Field'];
	      if (!in_array($field, array('nid', 'vid'))) {
	        $source_fields[$field] = t('(From node abstract) @field', array('@field' => $field));
	      }
	    }
    }

    $source_fields += $this->_source_fields_field_upload();

//    $source_fields += parent::sourceFieldsCCK();
    return $source_fields;
  }

  public function source($content_type) {
    return new MigrateSourceSQL($this->query($content_type), $this->sourceFields($content_type), null, array('map_joinable' => FALSE));
  }

  public function destination($bundle) {
    return new MigrateDestinationNode($bundle);
  }

  //map provides the schema for the primary key of the source table.  this lets migrate associate a unique source id
  //with a migrated object
  public function map($content_type) {
    return new MigrateSQLMap($this->machineName,
      array(
      	'nid' => array(
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
          'description' => 'D6 ' . $content_type . ' ID.',
          'alias' => 'n',
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );
  }



  /**
   * Implements prepareRow
   *
   * Fetches path from {url_alias}
   */
  function prepareRow($current_row) {

    //check that destination vsite exists.  some vsites have been removed, but their content still exists
    $query = db_select('node', 'n')
      ->fields('n')
      ->condition('type', 'vsite', '=')
      ->condition('nid', $current_row->group_nid, '=');

    if (!$query->execute()->fetchAssoc()) {
      $os_migrate_legacy_site = variable_get('os_migrate_legacy_site', '');
      if (strlen($os_migrate_legacy_site)) {
        $blank = array('attributes' => array('target'=>'_blank'));
        Migration::saveMessage(t('Vsite !gid does not exist.  Assuming node !nid is orphaned.',
          array('!gid' => l($current_row->group_nid, $os_migrate_legacy_site . '/node/' . $current_row->group_nid, $blank),
                 '!nid' => l($current_row->nid, $os_migrate_legacy_site . '/node/' . $current_row->nid, $blank))), Migration::MESSAGE_NOTICE);
      } else {
        Migration::saveMessage(t('Vsite @gid does not exist.  Assuming node @nid is orphaned.',
                                array('@gid' => $current_row->group_nid, '@nid' => $current_row->nid)), Migration::MESSAGE_NOTICE);
      }
      return FALSE;
    }


    //check if bundle exists.
    $info = field_info_bundles('node');
    if (!isset($info[$this->bundle])) {
      Migration::saveMessage(t('Could not migrate into non-existant bundle: @bundle', array('@bundle' => $this->bundle)));
      return FALSE;
    }

    //get path from {url_alias}
    $query = parent::d6_db_select('url_alias', 'ua')
      ->fields('ua', array('dst'))
      ->condition('ua.src', 'node/'.$current_row->nid, '=');
    $results = $query->execute()->fetchAssoc();
    $current_row->path = $results['dst'];

    //lanuage should be explicitly undefined instead of blank
    if (!strlen($current_row->language)) {
      $current_row->language = 'und';
    }

    //format.  legacy formats are Filtered HTML, Full HTML, PHP Code, Comments HTML, Minimal WYSIWYG, FUll WYSWIWYG
    switch ($current_row->format_name) {
      case 'PHP Code':
        $current_row->format = 'plain_text';
        break;
      default:
        $current_row->format = 'filtered_html';
    }

    //adjust the body to use new fids for inline objects
    parent::os_migrate_inline_update($current_row);

    $this->_prepare_file_field($current_row);

    return parent::prepareRow($current_row);
  }

  //helper functions for common file field

  function _has_file_field() {
    $fields = field_info_instances('node', $this->bundle);
    return (isset($fields['field_upload']));
  }

  function _prepare_file_field(&$row) {
    if ($this->_has_file_field()) {
      $files_query = parent::os_migrate_node_get_files_query($row->vid);
      $fields = array('description' => 'description', 'list' => 'display', 'filepath' => 'field_upload');
      parent::os_migrate_prepare_files($files_query->execute(), $row, 'field_upload', $fields);
    }
  }

  function _map_file_field() {
    if ($this->_has_file_field()) {
      $this->addFieldMapping('field_upload', 'field_upload')
      ->defaultValue(NULL);
      $this->addSimpleMappings(parent::subfields('field_upload', array('language', 'description', 'display',)));
      $this->addFieldMapping('field_upload:destination_file', 'filepath');
      $this->addFieldMapping('field_upload:source_dir')
        ->defaultValue(variable_get('os_migrate_files', FALSE));
      //good defaults
      $this->addUnmigratedDestinations(parent::subfields('field_upload', array('file_class', 'destination_dir', 'file_replace', 'preserve_files',)));
    }
  }

  function _source_fields_field_upload() {
    if ($this->_has_file_field()) {
      return array(
        'field_upload' => t('Uploaded file'),
        'field_upload:description' => t('Uploaded file\'s desctription'),
        'field_upload:display' => t('Include uploaded file in listing'),
        'field_upload:language' => t('Uploaded file\'s langauge'),
        'filepath' => t('Full path to file'),
      );
    } else {
      return array();
    }
  }

}
