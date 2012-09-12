<?php

/**
 * @file os.autocomplete.inc
 * OpenScholar's autocomplete functions.
 */

/**
 * Returns all nodes in space, all nodes if outside of space.
 */
function os_node_autocomplete($str) {

  // initialize view to generate sessions output when viewing a conference.
  $view = views_get_view('vsite_autocomplete');
  $view->init();

  $view->set_display('default');
  $view->set_items_per_page(0);
  $view->execute();

  $matches = array();
  foreach ($view->result as $node) {
    if (stristr($node->node_title, $str)) {
     $matches[$node->node_title. ' [nid:' . $node->nid . ']'] = check_plain($node->node_title);
    }
  }

  drupal_json_output($matches);
}
