<?php

class os_boxes_media extends os_boxes_default {

  public function options_defaults() {
    $options = parent::options_defaults();
    $options['fid'] = 0;
    return $options;
  }

  public function options_form(&$form_state) {
    $form = parent::options_form($form_state);

    $form['fid'] = array(
      '#title' => t('File or HTML'),
      '#type' => 'media',
      '#default_value' => array('fid' => $this->options['fid']),
      '#description' => t('Choose, enter or link to the media you wish to embed.')
    );

    $form['help'] = array(
      '#prefix' => '<p>',
      '#markup' => t('If you\'d like additional text around your image, video or iframe, you should use the Custom Text/HTML box. It will allow you to embed media around text.'),
      '#suffix' => '</p>',
    );

    return $form;
  }

  public function render() {
    $block = parent::render();

    if ($file = file_load($this->options['fid'])) {
      $block['content'] = drupal_render(file_view($file));
    }
    else {
      $block['content'] = t('No media given.');
    }
    return $block;
  }
}