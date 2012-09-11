<?php
/**
 * @file
 * os_blog.box.inc
 */

/**
 * Implements hook_default_box().
 */
function os_blog_default_box() {
  $export = array();

  $box = new stdClass();
  $box->disabled = FALSE; /* Edit this to true to make a default box disabled initially */
  $box->api_version = 1;
  $box->delta = 'blog_rss_feed';
  $box->plugin_key = 'os_boxes_rss';
  $box->title = '';
  $box->description = 'Blog RSS Feed';
  $box->options = array(
    'content' => array('blog'),
    'show_all' => TRUE,
  );
  $export['os_blog-blog_rss_feed'] = $box;

  return $export;
}
