<?php
// $Id$


/**
 * @file
 * os_reader.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function os_reader_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  /* Edit this to true to make a default context disabled initially */
  $context->disabled = FALSE;
  $context->api_version = 3;
  $context->name = 'reader_reader';
  $context->description = '';
  $context->tag = 'OS features';
  $context->conditions = array(
    'node' => array(
      'values' => array(
        'feed' => 'feed',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
    'path' => array(
      'values' => array(
        'reader' => 'reader',
        'reader/*' => 'reader/*',
      ),
    ),
    'views' => array(
      'values' => array(
        'reader' => 'reader',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'views-reader-block' => array(
          'module' => 'views',
          'delta' => 'reader-block',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'boxes-os_taxonomy_fltr_cat' => array(
          'module' => 'boxes',
          'delta' => 'os_taxonomy_fltr_cat',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
        'boxes-os_addthis' => array(
          'module' => 'boxes',
          'delta' => 'os_addthis',
          'region' => 'sidebar_second',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('OS features');
  $export['reader_reader'] = $context;

  return $export;
}

