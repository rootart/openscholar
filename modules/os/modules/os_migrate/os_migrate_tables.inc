<?php

/**
 * @file
 * Implements TableCopyMigration for several direct table copies
 */

class IQSSPinserverRegisterTableMigration extends TableCopyMigration {

  public function __construct() {
    $this->required_modules = array('iqss_pinserver_register');
    $key_schema = array(
        'uid' => array(
            'type' => 'int',
            'unsigned' => TRUE,
            'not null' => TRUE,
        ),
    );

    $table = 'iqss_pinserver_register';
    parent::__construct($table, $key_schema);
  }
}


class CitationDistributeTableMigration extends TableCopyMigration {
  public function __construct() {
    $this->required_modules = array('citation_distribute');

    $key_schema = array(
      'nid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'plugin' => array(
        'type' => 'varchar',
        'length' => 64,
        'not null' => TRUE,
      ),
    );

    $table = 'citation_distribute';

    parent::__construct($table, $key_schema);
  }
}
