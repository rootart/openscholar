<?php

/**
 * @file
 * Abstract class for copying tables
 */

abstract class TableCopyMigration extends OSMigration {
  /**
   * @function __construct
   * @param string $table
   * 	Name of table to copy.
   * @param array $key_schema
   *  Scheme of primary key
   *
   *  i.e.
   *  array(
   *   'rid' => array(
   *     'type' => 'int',
   *     'unsigned' => TRUE,
   *     'not null' => TRUE,
   *   ),
   *  );
   */
  public function __construct($table, $schema, $group=NULL, $dest_table=NULL, $count_query = NULL) {
    if ($group) {
      parent::__construct($group);
    } else {
      parent::__construct();
    }


    $this->table = $table;
    $this->alias = $table[0];
    $this->dest_table = ($dest_table) ? $dest_table : $table;
    //$query = parent::d6_db_select($table, $this->alias)->fields($this->alias);
    $map_schema = $schema;
    foreach($map_schema as $key => $key_schema) {
      $map_schema[$key]['alias'] = $this->alias;
    }

    $this->source = new MigrateSourceSQL($this->query(), array(), $count_query, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationTableCopy($this->dest_table, $schema);
    $this->map = new MigrateSQLMap($this->machineName, $map_schema, $schema);

  }

  function query() {
    return parent::d6_db_select($this->table, $this->alias)
      ->fields($this->alias);
  }
}
