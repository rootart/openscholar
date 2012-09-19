<?php
/**
 * @file
 * os_classes.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function os_classes_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'os_classes';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'OS Classes';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Classes';
  $handler->display->display_options['access']['type'] = 'spaces_feature';
  $handler->display->display_options['access']['spaces_feature'] = 'os_classes';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '15';
  $handler->display->display_options['style_plugin'] = 'default';
  $handler->display->display_options['row_plugin'] = 'node';
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
  /* Sort criterion: Content: Sticky */
  $handler->display->display_options['sorts']['sticky']['id'] = 'sticky';
  $handler->display->display_options['sorts']['sticky']['table'] = 'node';
  $handler->display->display_options['sorts']['sticky']['field'] = 'sticky';
  $handler->display->display_options['sorts']['sticky']['order'] = 'DESC';
  /* Sort criterion: Content: Offered (field_offered_year) */
  $handler->display->display_options['sorts']['field_offered_year_value']['id'] = 'field_offered_year_value';
  $handler->display->display_options['sorts']['field_offered_year_value']['table'] = 'field_data_field_offered_year';
  $handler->display->display_options['sorts']['field_offered_year_value']['field'] = 'field_offered_year_value';
  $handler->display->display_options['sorts']['field_offered_year_value']['order'] = 'DESC';
  /* Sort criterion: Content: Semester (field_semester) */
  $handler->display->display_options['sorts']['field_semester_value']['id'] = 'field_semester_value';
  $handler->display->display_options['sorts']['field_semester_value']['table'] = 'field_data_field_semester';
  $handler->display->display_options['sorts']['field_semester_value']['field'] = 'field_semester_value';
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'class' => 'class',
  );
  /* Filter criterion: Spaces: Content in current space */
  $handler->display->display_options['filters']['current']['id'] = 'current';
  $handler->display->display_options['filters']['current']['table'] = 'spaces';
  $handler->display->display_options['filters']['current']['field'] = 'current';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'classes';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Classes';
  $handler->display->display_options['menu']['name'] = 'primary-menu';
  $export['os_classes'] = $view;

  return $export;
}
