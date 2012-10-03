<?php

function cp_feature_customize($js, $feature_name) {
  if (!$js) {
    drupal_goto('features/'. $feature_name);
    return;
  }

  ctools_include('modal');
  ctools_include('ajax');
  $feature = feature_load($feature_name);
  $feature_title = features_get_feature_title($feature);

  $form_state = array(
    'title' => $feature_title,
    'ajax' => TRUE,
    'build_info' => array(
      'args' => array($feature_name),
    ),
  );

  $output = ctools_modal_form_wrapper('cp_feature_customize_form', $form_state);
  if ($form_state['submitted']) {
    $output[] = ctools_modal_command_dismiss();
  }
  else {
    $output[] = ajax_command_append('#modal-content div.buttons','<input class="form-submit close" onClick="$(\'div.modal-header a.close\').click(); return false;" value="Cancel" />');
    $output[] = ajax_command_after('#modal-content #edit-reset','<input class="form-submit close" onClick="$(\'div.modal-header a.close\').click(); return false;" value="Cancel" />');
  }

  echo ajax_render($output);
  exit;
}

/**
 * Wrapper form function
 * Feature customization pages all pass through here
 * Any modules that want to alter all of them can do so by altering this form
 */
function cp_feature_customize_form($form, $form_state, $feature) {
  $item = menu_get_item('features/'. $feature);
  $form['#attributes'] = array(
    'class' => array(
      $feature.'-customize-form',
    )
  );
  if ($item['path'] == 'features/'.$feature) {
    $form_id = array_shift($item['page_arguments']);
    if ($file = $item['include_file']) {
      require_once(DRUPAL_ROOT . '/' .$file);
    }

    if (count($item['page_arguments'])) {
      $form_state['build_info']['args'] = $item['page_arguments'];
    }

    $form += drupal_retrieve_form($form_id, $form_state);
  }
  else {
    $form['no_form'] = array(
      '#prefix' => '<p>',
      '#markup' => t('This feature has no settings.'),
      '#suffix' => '</p>',
    );
    $form['ok'] = array(
      '#type' => 'submit',
      '#value' => t('Ok'),
    );
    $form['#after_build'][] = '_cp_feature_customize_remove_no_form';
  }

  //Store the current feature in the form data
  $form['#feature'] = $feature;

  return $form;
}

function _cp_feature_customize_remove_no_form($element, $form_state) {
  $keys = array('no_form', 'ok', 'form_build_id', 'form_token', 'form_id');
  $children = element_children($element);
  $new = array_diff($children, $keys);
  if (count($new) != 0) {
    unset($element['no_form'], $element['ok']);
  }

  return $element;
}