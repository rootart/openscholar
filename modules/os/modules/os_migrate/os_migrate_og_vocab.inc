<?php

/**
 * @file
 * Migrate plugin to populate the OG vocabulary field by the term
 * references.
 */

class OsOgVocabMigrate extends DynamicMigration {

  public function __construct(array $arguments) {
    $this->arguments = $arguments;
    parent::__construct();
    $this->bundle = $bundle = $arguments['bundle'];

    $this->description = t('Populate the OG vocabulary field from a remote Drupal 6 source.');

    $this->dependencies = array('OsOgVocabMigratePrepare');

    // Indicate we're updating existing data.
    $this->systemOfRecord = Migration::DESTINATION;

    $query = OSMigration::d6_db_select('term_node', 'tn');
    $query->innerJoin('og_vocab', 'ogv', 'tn.vid = ogv.vid');
    $query->innerJoin('node', 'n', 'tn.nid = n.nid');

    $query
      ->fields('tn')
      ->condition('n.type', $bundle)
      ->orderBy('n.nid');

    $this->query = $query;
    $this->map = new MigrateSQLMap($this->machineName, MigrateDestinationNode::getKeySchema(), MigrateDestinationNode::getKeySchema());

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($this->query);
    $this->destination = new MigrateDestinationNode($bundle);

    $this->addFieldMapping('nid', 'nid');
    $this->addFieldMapping(OG_VOCAB_FIELD, OG_VOCAB_FIELD);
  }


  public function prepareRow($row) {
    // We don't have the fields in the $row, so we load the entity to get
    // them.
    $wrapper = entity_metadata_wrapper('node', $row->nid);
    $tids = array();
    foreach ($field_names as $field_name) {
      $field = field_info_field($field_name);
      $vocabulary = taxonomy_vocabulary_machine_name_load($field['settings']['allowed_values'][0]['vocabulary']);
      $this->addOgVocab($vocabulary->vid, $field_name);
      if ($values = $wrapper->{$field_name}->value(array('identifier' => TRUE))) {
        $values = is_array($values) ? $values : array($values);
        $tids = array_merge($tids, $values);
      }
    }

    $row->{OG_VOCAB_FIELD} = $tids;
    parent::prepareRow($row);
  }

  /**
   * Construct the machine name from the source file name.
   */
  protected function generateMachineName($class_name = NULL) {
    return drupal_strtolower('OsOgVocabMigrate' . ucfirst($this->arguments['bundle']));
  }

  /**
   * Add the OG-vocab field to the bundle.
   */
  public function preImport() {
    $bundle = $this->arguments['bundle'];

    // Add OG-vocab field to bundle.
    if (!field_info_instance('node', OG_VOCAB_FIELD, $bundle)) {
      og_create_field(OG_VOCAB_FIELD, 'node', $bundle);
      self::displayMessage(t('Added "OG vocabulary" field to @bundle content-type.', array('@bundle' => $bundle)), 'success');
    }
  }

  /**
   * Helper function to add the OG-vocab entity.
   *
   * @param $vid
   *   The vocabulary ID related to the group.
   * @param $field_name
   *   The taxonomy term reference field, that needs to be converted to
   *   and OG-vocab entity.
   */
  protected function addOgVocab($vid, $field_name) {
  }
}
