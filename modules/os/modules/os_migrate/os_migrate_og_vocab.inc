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

    $query = OSMigration::d6_db_select('term_node', 'tn');
    $query->innerJoin('og_vocab', 'ogv', 'tn.vid = ogv.vid');
    $query->innerJoin('vocabulary', 'v', 'tn.vid = v.vid');
    $query->innerJoin('node', 'n', 'tn.nid = n.nid');

    $query
      ->fields('tn')
      ->fields('v', array('multiple', 'required', 'tags'))
      ->condition('n.type', $bundle)
      ->orderBy('n.nid');

    $source_key = MigrateDestinationNode::getKeySchema();
    // Set the alias, so the query in MigrateSourceSQL::performRewind()
    // will not fail.
    $source_key['nid']['alias'] = 'n';

    $this->query = $query;
    $this->map = new MigrateSQLMap($this->machineName, $source_key, MigrateDestinationNode::getKeySchema());

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($this->query);
    $this->destination = new MigrateDestinationNode($bundle);

    $this->addFieldMapping('nid', 'nid');
  }


  public function prepareRow($row) {
    $this->addOgVocab($row);
    $row->{OG_VOCAB_FIELD}[] = $row->tid;
    parent::prepareRow($row);
  }

  /**
   * Add an OG vocab entity, if needed.
   */
  private function addOgVocab($row) {
    $term = taxonomy_term_load($row->tid);
    $bundle = $this->arguments['bundle'];

    if (!og_vocab_load_og_vocab($term->vid, 'node', $bundle)) {
      // Create an OG vocab, based on the vocabulary settings.

      $settings = array(
        'required' => $row->required,
        'widget_type' => $row->tags ? 'entityreference_autocomplete_tags' : 'options_select',
        'cardinality' => $row->multiple,
      );
      $og_vocab = og_vocab_create_og_vocab($term->vid, 'node', $bundle, $settings);
      $og_vocab->save();
    }
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
