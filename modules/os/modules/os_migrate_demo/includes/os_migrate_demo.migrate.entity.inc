<?php

/**
 * @file
 * Populate distribution with some content.
 */

abstract class OsMigrate extends Migration {

  public function __construct() {
    parent::__construct();

    // Make sure we can use it for node and term only.
    if (!in_array($this->entityType, array('node', 'taxonomy_term'))) {
      throw new Exception('OsMigrate supports only nodes and terms.');
    }

    $this->description = t('Import @type - @bundle from CSV file.', array('@type' => $this->entityType, '@bundle' => $this->bundle));

    if ($this->entityType == 'node') {
      $this->addFieldMapping('title', 'title');
      $class_name = 'MigrateDestinationNode';
    }
    elseif ($this->entityType == 'taxonomy_term') {
      $this->addFieldMapping('name', 'name');
      $class_name = 'MigrateDestinationTerm';
    }

    // Create a map object for tracking the relationships between source rows
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap($this->machineName, $key, MigrateDestinationEntityAPI::getKeySchema($this->entityType));

    // Create a MigrateSource object.
    $this->source = new MigrateSourceCSV(drupal_get_path('module', 'os_migrate_demo') . '/csv/' . $this->entityType . '/' . $this->bundle . '.csv', $this->csvColumns, array('header_rows' => 1));
    $this->destination = new $class_name($this->bundle, array('text_format' => 'filtered_html'));
  }

  function prepareRow($row) {
    $row->uid = 1;
  }
}
