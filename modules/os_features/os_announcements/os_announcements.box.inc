<?php
// $Id$


/**
 * @file
 * os_announcements.box.inc
 */

/**
 * Implements hook_default_box().
 */
function os_announcements_default_box() {
  $export = array();

  $box = new stdClass();
  /* Edit this to true to make a default box disabled initially */
  $box->disabled    = FALSE;
  $box->api_version = 1;
  $box->delta       = 'os_announcements_latest';
  $box->plugin_key  = 'os_sv_list_box';
  $box->title       = 'Latest Announcements';
  $box->description = 'Latest Announcements';
  $box->options     = array(
    'content_type' => 'announcement',
    'display' => 'display_title',
    'more' => 1,
    'more_text' => 'More',
    'more_link' => 'announcements',
    'pager' => 0,
    'number_of_items' => '5',
    'sort_by' => 'sort_newest',
  );
  $export['os_announcements_latest'] = $box;

  return $export;
}

