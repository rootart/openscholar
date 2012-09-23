<?php

/**
 * @file os_migrate_node_softare.inc
 */

class SoftwareProjectNodeOSMigration extends AbstractNodeOSMigration {
  public function __construct() {

    /**
     * AbstractNodeOSMigration's __construct() will set up most of the common node migration.
     */
    parent::__construct('software_project', 'scholar_software_project'); //args are d7 bundle, d6 content type.

    $this->description = t('Software Project');

    //$this->dependencies[] = 'StubNodeOS'; // append dependencies.  these classes must be migrated before this class.

    //method
    $this->addFieldMapping('field_software_method', 'field_scholar_software_method_value');

    //rbuild shortname
    $this->addFieldMapping('field_rbuild_short_name', 'field_scholar_software_name_value');
    $this->addFieldMapping('field_rbuild_short_name:format')->defaultValue('plain_text');
    $this->addFieldMapping('field_rbuild_short_name:language', 'language');

    //repo
    $this->addFieldMapping('field_software_repo', 'field_scholar_software_repo_value');
    $this->addFieldMapping('field_software_repo:format')->defaultValue('plain_text');
    $this->addFieldMapping('field_software_repo:language', 'language');


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

    return parent::prepareRow($current_row); //false -> skip this row

  }
}

class SoftwareReleaseNodeOSMigration extends AbstractNodeOSMigration {
  public function __construct() {

    /**
     * AbstractNodeOSMigration's __construct() will set up most of the common node migration.
     */
    parent::__construct('software_release', 'scholar_software_release'); //args are d7 bundle, d6 content type.

    $this->description = t('Software release');

    $this->dependencies[] = 'SoftwareProjectNodeOS';

    //project entity
    $this->addFieldMapping('field_software_project', 'field_scholar_software_project_nid');

    //recommended
    $this->addFieldMapping('field_software_recommended', 'field_scholar_software_recommend_value');

    //version
    $this->addFieldMapping('field_software_version', 'field_software_version');
    $this->addFieldMapping('field_software_version:format')->defaultValue('plain_text');
    $this->addFieldMapping('field_software_version:language', 'language');

    //file package
    $this->addFieldMapping('field_software_package', 'field_software_package')->defaultValue(NULL);
    $this->addFieldMapping('field_software_package:language', 'language');
    $this->addFieldMapping('field_software_package:source_dir')->defaultValue(variable_get('os_migrate_files', FALSE));
    ;
    $this->addFieldMapping('field_software_package:destination_file', 'field_software_package:destination_file');
    $this->addFieldMapping('field_software_package:description', 'field_scholar_software_file_data');
    $this->addFieldMapping('field_software_package:display', 'field_scholar_software_file_list');

    $this->addUnmigratedDestinations(parent::subfields('field_software_package', array('destination_dir', 'preserve_files', 'file_class', 'file_replace')));

    $this->removeFieldMapping('title');
  }

  public function query($content_type) {
    $query = parent::query($content_type);

    $query->join('files', 'f', 'f.fid = ct.field_scholar_software_file_fid');
    $query->addField('f', 'filepath');
    $query->condition('f.filepath', 'http://r.iq.%', 'NOT LIKE');

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
    $subfields = parent::subfields('field_software_package', array('destination_file', 'file_class', 'file_replace'));
    $subfields[] = 'field_software_version';
    $subfields[] = 'field_software_package';
    $fields += array_combine($subfields, $subfields);

    return $fields;
  }

  public function prepareRow(&$current_row) {

    if ($description = @unserialize($current_row->field_scholar_software_file_data)) {
      $current_row->field_scholar_software_file_data = $description;
    }

    $current_row->field_software_version = $current_row->field_versionmajor_value;
    if (strlen($current_row->field_versionextra_value)) {
      $current_row->field_software_version .= ':' . $current_row->field_versionextra_value;
    }
    if (strlen($current_row->field_versionpatch_value)) {
      $current_row->field_software_version .= ':' . $current_row->field_versionpatch_value;
    }

    $file = parent::get_file_by_fid($current_row->field_scholar_software_file_fid);
    if ($file) {
      parent::os_migrate_prepare_files(array($file), $current_row, 'field_software_package');
      $current_row->{'field_software_package:destination_file'} = $current_row->filepath;

    }

    return parent::prepareRow($current_row); //false -> skip this row

  }

}