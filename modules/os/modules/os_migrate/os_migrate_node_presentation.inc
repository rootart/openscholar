<?php

/**
 * @file os_migrate_node_presentation.inc
 */

class PresentationNodeOSMigration extends AbstractNodeOSMigration {
  public function __construct() {

    /**
     * AbstractNodeOSMigration's __construct() will set up most of the common node migration.
     */
    parent::__construct('presentation'); //args are d7 bundle, d6 content type.

    $this->description = t('Presentation migration class.');

    //location
    $this->addFieldMapping('field_presentation_location', 'field_presentation_location_value');
    $this->addFieldMapping('field_presentation_location:format')->defaultValue('plain_text');
    $this->addFieldMapping('field_presentation_location:language', 'language')->defaultValue(LANGUAGE_NONE);

    //date
    $this->addFieldMapping('field_presentation_date', 'field_presentation_date_value');
    $this->addFieldMapping('field_presentation_date:to', 'field_presentation_date_value');
    $this->addUnmigratedDestinations(parent::subfields('field_presentation_date', array('timezone', 'rrule', 'from')), t('DNM'));

    //file
    $this->addFieldMapping('field_presentation_file', 'field_presentation_file');
    $this->addFieldMapping('field_presentation_file:og_group_ref', 'group_nid');
    $this->addSimpleMappings(parent::subfields('field_presentation_file', array('destination_file', 'description', 'display')));
    $this->addFieldMapping('field_presentation_file:source_dir')
      ->defaultValue(variable_get('os_migrate_files') . '/' . parent::d6_variable_get('file_directory_path', 'sites/default/files'));
    $this->addFieldMapping('field_presentation_file:destination_dir');
    $this->addUnmigratedDestinations(parent::subfields('field_presentation_file', array('file_class', 'language', 'destination_dir', 'file_replace', 'preserve_files')));

    //$this->dependencies[] = 'StubNodeOS'; // append dependencies.  these classes must be migrated before this class.




  }

  public function query($content_type) {
    $query = parent::query($content_type);

    /**
     * Modify the query.  Add joins and fields here.  content_type_${content_type} and its fields are already included.
     */

    return $query;
  }


  public function sourceFields($content_type) {
    $fields = parent::sourceFields($content_type);

    /**
     * Describe each of the source columns
     */

    return $fields;
  }

  public function prepareRow(&$current_row) {

    /**
     * This method is called while looping over the results of the query provided earlier.
     *
     * This is your chance to munge data.  (See Abstract's filter formats).
     *
     * This is also your chance to query additional tables that didn't make sense to join earlier
     * (for example, see gallery.  d6's gallery had references to image nodes.  d7's gallery uses
     * file fields for images.  While looping over d6 gallery nodes, prepareRow uses additional queries
     * to get those image nodes and extract everything needed by file fields.)
     */

    //need to get files from content_field_presentation_file
    $query = $this->_presentation_files_query($current_row->nid);
    $fields = array('filepath' => 'destination_file', 'data'=>'description', 'list' => 'display');
    parent::os_migrate_prepare_files($query->execute(), $current_row, 'field_presentation_file', $fields);

    if (list($path, $name) = $this->_migrate_filename($current_row->{'field_presentation_file:filepath'})) {
      $current_row->{'field_presentation_file:filename'} = $name;
      $current_row->{'field_presentation_file:destination_dir'} = 'public://' . $path;
      $current_row->{'field_presentation_file'} = $path . '/' . $name;
    }

    return parent::prepareRow($current_row); //false -> skip this row

  }

  //returns a query to fetch presentation files from legacy db
  function _presentation_files_query($nid) {
    $query = parent::d6_db_select('content_field_presentation_file', 'ct')
      ->condition('nid', $nid)
      ->fields('ct');

    $query->join('files', 'f', 'f.fid = ct.field_presentation_file_fid');
    $query->fields('f', array('filepath'));

    return $query;
  }
}