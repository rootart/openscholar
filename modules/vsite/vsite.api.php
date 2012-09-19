<?php

/**
 * @file
 * vsite module API file.
 */

/**
 * Implements hook_vsite_og_node_type_info().
 *
 * Notify vsite module which nodes bundle types are group or a group/content.
 */
function hook_vsite_og_node_type_info() {
  return array(
    'group1' => 'group',
  );
}
