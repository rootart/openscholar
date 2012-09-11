<?php
// $Id$


/**
 * @file
 * os_image_gallery.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function os_image_gallery_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  /* Edit this to true to make a default context disabled initially */
  $context->disabled = FALSE;
  $context->api_version = 3;
  $context->name = 'gallery_gallery';
  $context->description = 'Image gallery or image page';
  $context->tag = 'OS features';
  $context->conditions = array(
    'menu' => array(
      'values' => array(
        'galleries' => 'galleries',
      ),
    ),
    'node' => array(
      'values' => array(
        'media_gallery' => 'media_gallery',
      ),
      'options' => array(
        'node_form' => '0',
      ),
    ),
    'path' => array(
      'values' => array(
        'galleries' => 'galleries',
        'galleries/*' => 'galleries/*',
      ),
    ),
    'views' => array(
      'values' => array(
        'os_image_gallery' => 'os_image_gallery',
        'os_image_gallery_slideshow' => 'os_image_gallery_slideshow',
      ),
    ),
  );
  $context->reactions = array();
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Image gallery or image page');
  t('OS features');
  $export['gallery_gallery'] = $context;

  return $export;
}

