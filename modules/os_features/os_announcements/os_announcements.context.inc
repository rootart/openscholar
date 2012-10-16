<?php
// $Id$


/**
 * @file
 * os_announcements.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function os_announcements_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  /* Edit this to true to make a default context disabled initially */
  $context->disabled = FALSE;
  $context->api_version = 3;
  $context->name = 'announcements_announcements';
  $context->description = 'Context that is active in the announcements section of your site';
  $context->tag = 'OS features';
  $context->conditions = array(
    'menu' => array(
      'values' => array(
        'announcements' => 'announcements',
      ),
    ),
    'node' => array(
      'values' => array(
        'announcement' => 'announcement',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
    'path' => array(
      'values' => array(
        '/announcements' => '/announcements',
      ),
    ),
    'views' => array(
      'values' => array(
        'os_announcements:page' => 'os_announcements:page',
      ),
    ),
  );
  $context->reactions = array(
    'menu' => 'announcements',
    'block' => array(
      'blocks' => array(
        'boxes-os_announcements_latest' => array(
          'module' => 'boxes',
          'delta' => 'os_announcements_latest',
          'weight' => -21,
          'region' => 'sidebar_second',
          'label' => 'Latest Announcements',
        ),
        'boxes-os_addthis' => array(
          'module' => 'boxes',
          'delta' => 'os_addthis',
          'label' => 'AddThis Button',
          'weight' => 50,
          'region' => 'sidebar_second',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Context that is active in the announcements section of your site');
  t('OS features');
  $export['announcements_announcements'] = $context;

  return $export;
}

