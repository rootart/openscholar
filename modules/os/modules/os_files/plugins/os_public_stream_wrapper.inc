<?php

/**
 * Extension of Drupal public (public://) stream wrapper class.
 *
 * Provides support for storing publicly accessible files with vsite paths
 * withing the Drupal file interface.
 */
class OSPublicStreamWrapper extends DrupalPublicStreamWrapper {

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URI of a private file.
   */
  function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());

    $prefix = vsite_get_purl();
    if (!empty($prefix)) {
      $pieces = explode("/", $path);
      if ($pieces[0] == $prefix) {
        // Remove vsite component.
        array_shift($pieces);
        // Use Vsite files menu entry (url will automatically prefix with purl).
        return url('files/' . implode("/", $pieces), array('absolute' => TRUE));
      }
    }

    // Use the default Public URL.
    return parent::getExternalUrl();
  }
}
