<?php
// $Id$


/**
 * Biocv settings form
 */
function os_biocv_settings() {
  $options = array(
    0 => t('Display as a separate section with configurable text and PDF attachment'),
    1 => t('Display as a PDF link on my Bio')
  );

  $upload_link = l(t('upload a PDF'), 'node/add/cv');
  $node = os_biocv_get_cv_node();
  if ($node !== FALSE) {
    $upload_link = l(t('upload a PDF'), 'node/' . $node->nid . '/edit');
  }
  $description = t('Hint: ') . $upload_link;

  $filepath = _os_biocv_get_cv_filepath($node);

  if ($filepath !== FALSE) {
    $edit_link = l(t('remove or replace'), 'node/' . $node->nid . '/edit');
    $description = t('You may @edit_link your PDF Upload', array('@edit_link' => $edit_link));
  }

  $form['os_biocv_use_pdf_as_cv'] = array(
    '#type' => 'radios',
    '#weight' => -10,
    '#title' => t('Curriculum Vitae Display'),
    '#default_value' => variable_get('os_biocv_use_pdf_as_cv', 0),
    '#options' => $options,
    '#description' => $description,
  );

  return system_settings_form($form);
}

