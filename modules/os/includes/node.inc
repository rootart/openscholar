<?php
// $Id$


/**
 * @file
 * Common functions for working with node content in OpenScholar.
 */

/* @todo both of these attempts at an API EFQ fail -oren
 *
 * PDOException: SQLSTATE[42S22]: Column not found: 1054 Unknown column 'ogm.bundle' in 'where clause': SELECT DISTINCT ogm.entity_type AS entity_type, ogm.etid AS entity_id, node.vid AS revision_id, node.type AS bundle FROM {og_membership} ogm INNER JOIN {node} node ON node.nid = ogm.etid INNER JOIN {node} node_2 ON node.nid = ogm.etid WHERE (ogm.gid = :db_condition_placeholder_0) AND (node.status = :db_condition_placeholder_1) AND (ogm.entity_type = :db_condition_placeholder_2) AND (ogm.bundle = :db_condition_placeholder_3) ORDER BY node.nid ASC; Array ( [:db_condition_placeholder_0] => 1 [:db_condition_placeholder_1] => 1 [:db_condition_placeholder_2] => node [:db_condition_placeholder_3] => bio ) in field_sql_storage_field_storage_query() (line 582 of /Applications/MAMP/htdocs/openscholar/modules/field/modules/field_sql_storage/field_sql_storage.module).
 *
class OpenscholarNodeFieldQuery extends EntityFieldQuery {
  public function __construct($type = NULL) {
    // No constructor for EntityFieldQuery
    // parent::__construct();

    $this
      ->entityCondition('entity_type', 'node')
      ->propertyCondition('status', 1);

    if (isset($type) && strlen($type) > 0) {
      $this->entityCondition('bundle', $type);
    }

    if (!module_exists('vsite')) {
      return;
    }

    $space = spaces_get_space();
    if (!is_numeric($space->id)) {
      return;
    }

    $this
      ->fieldCondition(OG_AUDIENCE_FIELD, 'target_id', $space->id);
  }
}

/**
 * Returns a new EntityFieldQuery in the current space, if vsite is enabled.
 *
function _os_node_query($type = '') {
  // Prepares a new EntityFieldQuery for nodes.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node');

  // Adds node type condition if given
  if (isset($type) && strlen($type) > 0) {
    dpm($type);
    $query->entityCondition('bundle', $type);
  }

  $query->propertyCondition('status', 1);

  // Returns if we are running a single-tenant installation.
  if (!module_exists('vsite')) {
    return $query;
  }

  // Returns if we cannot load the current space.
  $space = spaces_get_space();
  if (!is_numeric($space->id)) {
    return $query;
  }

  // Adds field condition for OG audience if available.
  $query->fieldCondition(OG_AUDIENCE_FIELD, 'target_id', $space->id);
  return $query;
} */