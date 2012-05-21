<?php

function cp_layout_full_form($form, $form_state, $ctx) {
  $form = array();
  ctools_include('ajax');
  ctools_include('modal');

  // Temporary: To be replaced by API function later
  $contexts = context_context_list();
  $ctx = $ctx?$ctx:'os_front';

  $form['context_selection'] = array(
    '#type' => 'select',
    '#title' => t('Site Section'),
    '#options' => $contexts,
    '#default_value' => $ctx,
    '#multiple' => false,
  );

  $use_content_regions = false;
  if ($ctx == 'os_front') {
    $use_content_regions = true;
  }
  $ctx = context_load($ctx);

  if ($ctx === FALSE) return $form;  // context doesn't exist. bail out

  $form['context'] = array(
    '#type' => 'value',
    '#value' => $ctx,
  );

  // temporary: to be replaced with api calls
  $plugin = context_get_plugin('reaction', 'block');

  // will need to be replaced if it doesn't work with spaces
  // no idea if it will or not
  $blocks = $plugin->get_blocks(null, $ctx);

  // filter out blocks we don't want
  // add info to existing blocks
  drupal_alter('layout_available_blocks', $blocks);


  $regions = array(
    'header_left',
    'header_main',
    'header_right',
    'menu_bar',
    'sidebar_first',
    'content',
    'sidebar_second',
    'footer',
    'content_top',
    'content_left',
    'content_right',
    'content_bottom',
  );

  $form['unused'] = array(
    '#theme_wrappers' => array('cp_layout_unused_widgets'),
    '#tags' => array(),
    '#factories' => array()
  );

  $form['layout'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'cp_layout').'/theme/cp_layout.js',
      ),
      'css' => array(
        drupal_get_path('module', 'cp_layout').'/theme/cp_layout.common.css',
      ),
    ),
    '#pre_render' => array(
      '_cp_layout_rearrange_content_regions',
    ),
  );

  if ($use_content_regions) {
    $form['layout']['#attributes'] = array(
    	'class' => array(
        'content-regions',
      ),
    );
  }

  foreach ($regions as $r) {
    $form['layout'][$r] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'cp-region',
          drupal_html_class('cp-region-'.$r),
        ),
      ),
      'widgets' => array(
        '#type' => 'hidden',
        '#default_value' => '',
      ),
    );
  }

  foreach ($blocks as $bid => $b) {
    // factory check
    if (strpos($bid, 'boxes_add__') !== FALSE) {
      $form['unused']['#factories'][$bid] = $b;
    }

    if ($b->region == -1) {
      $form['unused'][$bid] = array(
        '#theme' => 'cp_layout_widget',
        '#widget' => $b,
      );
    }
    elseif (isset($form['layout'][$b->region])) {
      $form['layout'][$b->region][$bid] = array(
        '#theme' => 'cp_layout_widget',
        '#widget' => $b,
      );
    }

    if (isset($b->tags)) {
      $tags = explode(', ', $b->tags);
      foreach ($tags as $t) {
        if (!in_array($t, $form['unused']['#tags'])) {
          $form['unused']['#tags'][] = $t;
        }
      }
    }
  }

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel' => array(
      '#type' => 'button',
      '#value' => t('Cancel'),
    ),
  );

  $form['#submit'][] = 'cp_layout_full_submit';

  return $form;
}

function cp_layout_ajax() {

}

function _cp_layout_rearrange_content_regions($elements) {
  foreach ($elements as $r => $region) {
    if (strpos($r, 'content_') !== FALSE) {
      unset($elements[$r]);
      $elements['content'][$r] = $region;
    }
  }

  return $elements;
}

function cp_layout_full_submit($form, &$form_state) {
  $regions = $form_state['values']['layout'];
  foreach ($regions as $r => $d) {
    $widgets = explode('|', $d['widgets']);
  }

  // merging with sitewide context? is that still something we want to do?

  // construct the data and save it
  // straight to context? into spaces? what?
}