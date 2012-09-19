<?php

class SpacesOverridesVariableMigration extends SpacesOverridesTableMigration {

  public function __construct() {
    $this->source_object = $this->dest_object = 'variable';
    parent::__construct();
  }

  public function query() {
    $query = parent::query();
    //$query->condition('object_id', 'spaces_features');

    $blacklist = array('vsite_design_disable_shield', 'vsite_shields_disable_shield');
    $query->condition('object_id', $blacklist, 'NOT IN');
    return $query;
  }

  public function prepareRow(&$row) {
    if (($value = @unserialize($row->value)) === FALSE) {
      if (empty($row->value) || $row->value == 'NULL') {
        return; //unless we have to update object_ids.
      }
      $value = $row->value; //serialize them all when we're done.
    }
    $object_id = $row->object_id;
    if ($this->_discontinued_variable($object_id)) {
      parent::saveMessage(t('Dropping variable: @var', array('@var' => $object_id)), MigrationBase::MESSAGE_INFORMATIONAL);
      return FALSE;
    }

    //flavor variables get new names
    if (preg_match('/_flavor$/', $row->object_id)) {
      //$this->_update_flavors($row);
      list($object_id, $value) = $this->_update_flavors($object_id, $value);
    }

    //scholar_publications variables:
    if (preg_match('/^scholar_publications/', $row->object_id)) {
      $object_id = $this->_update_variables_scholar_publications($row->object_id);
    }

    /*
     * pseudo-hook:  $this->_update_TYPE_ID($object_id, $value);
    * always returns array of paramters
    */
    $function = '_update_' . $row->object_type . '_' . $object_id;
    if (method_exists($this, $function)) {
      list($object_id, $value) = $this->$function($object_id, $value);
    }

    $row->object_id = $object_id;
    $row->value = serialize($value);
    return parent::prepareRow($row);
  }

  /*
   * @function _update_flavors
  *
  * Updates flavor variables and names.
  */
  public function _update_flavors($object_id, $value) {
    $object_id = preg_replace('/vsite_design_scholar/', 'os_appearance', $object_id);
    $value = preg_replace('/^scholar_/', '', $value);
    return array($object_id, $value);
  }

  public function _update_variables_scholar_publications($object_id) {
    return preg_replace('/^scholar_/', 'os_', $object_id);
  }
  /**
   * pseudo hooks (for lack of a better term)
   *
   * To save a little space and keep from writing a million checks for object id, I'm using the type
   * and id for the name of a variable function.  _update_TYPE_ID.
   *
   * Takes object_id and value.  Returns array of the same to be set with list().
   */

  /**
   * Update the spaces_features variable by fixing the module names.
   */
  public function _update_variable_spaces_features($object_id, array $value) {
    $spaces_features = array();
    foreach ($value as $feature => $enabled) {
      $spaces_features[$this->_update_module($feature)] = $enabled;
    }
    return array($object_id, $spaces_features);
  }

  /**
   * spaces_preset_og variable
   */
  public function _update_variable_spaces_preset_og($object_id, $value) {
    $presets = array(
      'scholar' => 'os_personal',
      'project' => 'os_department',
    );

    return (isset($presets[$value])) ? array($object_id, $presets[$value]) : FALSE;
  }

  //vsite_ga_google_analytics_id => os_ga_google_analytics_id
  public function _update_variable_vsite_ga_google_analytics_id($object_id, $value) {
    return array('os_ga_google_analytics_id', $value);
  }

  //vsite_shields_shield_info => os_shields_shield_info
  public function _update_variable_vsite_shields_shield_info($object_id, $value) {
    return array('os_shields_shield_info', $value);
  }

  // vsite_shields_shield => os_shields_shield
  public function _update_variable_vsite_shields_shield($object_id, $value) {
    return array('os_shields_shield', $value);
  }

  // check if a variable is discontinued
  public function _discontinued_variable($object_id) {
    $discontinued = array(
      'vsite_design_disable_shield',
      'vsite_shields_disable_shield',
    );
    return in_array($object_id, $discontinued);
  }

}



