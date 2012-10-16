<?php
function cp_theme_preprocess_overlay(&$vars) {
  // we never want these. They look awful
 $vars['tabs'] = false;
}

function cp_theme_preprocess_page(&$vars) {
  $vars['breadcrumb'] = '';
}