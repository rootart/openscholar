<?php

/**
 * @file
 *
 * Migration class
 **/

/**
 * User table migration class
 */
class UsersOSMigration extends OSMigration {
  public function __construct() {

    parent::__construct(MigrateGroup::getInstance('users'));
    $this->description = t('OS user accounts');

    /******************************************************
     *  Migrate the following fields from the users table *
     ******************************************************/


    $query = parent::d6_db_select('users', 'u')
      ->fields('u', array('uid','name','pass','mail','mode','sort','threshold','theme',
      									  'signature','signature_format','created','access','login',
      									  'status','timezone','language','picture','init','data',
      									  'timezone_name'))
      ->condition('u.name', array('admin'), 'NOT IN') //admin will already exist, so don't make a new one
      ->condition('u.uid', 1, '>');

    $source_fields = array(
      'uid' => t('User ID'),
      'roles' => t('List of a user\'s roles'),
      'field_first_name' => t('First name'),
      'field_first_name:format' => t('First name\'s language'),
      'field_first_name:language' => t('First name\'s format'),
      'field_last_name' => t('last name'),
      'field_last_name:format' => t('last name\'s language'),
      'field_last_name:language' => t('last name\'s format'),
    );
    $this->source = new MigrateSourceSQL($query, $source_fields, null, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationUser(array('md5_passwords'=>TRUE));
    //$this->dependencies = array(''); //permissions, og


    /***************************************************************************************
     *  Migrate needs to know about the keys of the source table to track migration status *
     ***************************************************************************************/


    $this->map = new MigrateSQLMap($this->machineName,
      array(
      	'uid' => array(
          'type' => 'int',
          'not null' => TRUE,
          'unsigned' => TRUE,
          'description' => 'D6 User ID.',
          'alias' => 'u',
        ),
      ),
      MigrateDestinationUser::getKeySchema()
    );


    /************************************************************************
     *  Field mappings tell Migrate which d7 column to deposit d6 data into *
     ************************************************************************/


    /* addSimpleMappings handles the columns that haven't changed or been renamed */
    $this->addSimpleMappings(array('uid','pass', 'name', 'created', 'access', 'login', 'status', 'init', 'theme',
                                   'signature', 'signature_format', 'language'));

    //hide email during development
    if (!variable_get('os_migrate_email_notification', FALSE)) {
      $this->addSimpleMappings(array('mail'));
    } else {
      $this->addUnmigratedSources(array('mail'), t('DNM'));
    }


    /* addFieldMapping lets you map one row to another or perform additional functions like setting a default */
    $this->addFieldMapping('timezone', 'timezone_name')->issueNumber(1791); //example issueNumber for linking to redmine
                                                                            //issues are per field, which makes little sense to me.
    $this->addFieldMapping('picture', 'picture')->defaultValue(0);
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('roles', 'roles'); //see prepareRow()

    //names
    $name_fields = array('field_first_name', 'field_last_name', 'field_first_name:format', 'field_last_name:format', 'field_first_name:language', 'field_last_name:language');
    $this->addSimpleMappings($name_fields);
    /* Unmapped source fields.  Mark them as Do Not Map, so the UI knows they're intentionally ignored */
    $this->addUnmigratedSources(array('mode','sort','threshold', 'timezone', 'data'), t('DNM'));

    /* Unmapped destination fields */
    $this->addUnmigratedDestinations(array('role_names', 'path', 'pathauto'), t('DNM')); //just ignore them for today

  }

  /**
   * Implements prepareRow
   *
   * `roles` isn't in the old DB's users table.  This function fetches it.
   **/
  public function prepareRow($current_row) {
    $query = parent::d6_db_select('users_roles', 'r')
      ->fields('r', array('uid', 'rid'))
      ->condition('r.uid', $current_row->uid, '=');

    $results = $query->execute();
    $current_row->roles = array('2' => '2'); //default role: authenticated user

    foreach($results as $row) {
      $current_row->roles[$row->rid] = $row->rid;
    }

    list ($fn, $ln) = $this->_get_full_name($row->uid);
    if ($fn) {
      $current_row->field_first_name = $fn;
      $current_row->{'field_first_name:format'} = 'plain_text';
      $current_row->{'field_first_name:language'} = LANGUAGE_NONE;
    }
    if ($ln) {
      $current_row->field_last_name = $ln;
      $current_row->{'field_last_name:format'} = 'plain_text';
      $current_row->{'field_last_name:language'} = LANGUAGE_NONE;
    }

    return TRUE;  // FALSE to skip a row.
  }

  /**
   * @function _get_full_name
   *
   * Gets the first and last name from vsite_user_profile
   * Should be a join, but there are duplicate profile nodes
   *
   * @param int $uid user id
   */
  function _get_full_name($uid) {
    $query = parent::d6_db_select('node', 'n')
      ->condition('type', 'vsite_users_profile')
      ->condition('uid', $uid);
    $query->join('content_type_vsite_users_profile', 'ct', 'n.nid = ct.nid');
    $query->fields('ct', array('vsite_users_first_name_value', 'vsite_users_last_name_value'));

    $results = $query->execute()->fetchAssoc();
    return (count($results)) ? array($results['vsite_users_first_name_value'], $results['vsite_users_last_name_value']) : array(FALSE, FALSE);
  }
}