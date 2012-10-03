<?php

/**
 * Implements hook_install_tasks().
 */
function openscholar_install_tasks($install_state) {
  $tasks = array();

  // Set the private file folder.
  $tasks['openscholar_private_file_system'] = array(
    'display_name' => t('Private file system'),
    'type' => 'form',
  );

  // OS flavors (production, development, etc)
  $tasks['openscholar_flavor_form'] = array(
    'display_name' => t('Choose a enviroment'),
    'type' => 'form'
  );

  // Simple form to select the installation type (single site or multitenant)
  $tasks['openscholar_install_type'] = array(
    'display_name' => t('Installation type'),
    'type' => 'form'
  );

  // If multitenant, we need to do some extra work, e.g. some extra modules
  // otherwise, skip this step
  $tasks['openscholar_vsite_modules_batch'] = array(
    'display_name' => t('Install supplemental modules'),
    'type' => 'batch',
    'run' => variable_get('os_profile_type', FALSE == 'vsite' || variable_get('os_profile_flavor', FALSE) == 'development') ? INSTALL_TASK_RUN_IF_NOT_COMPLETED : INSTALL_TASK_SKIP
  );

  return $tasks;
}

function openscholar_install_tasks_alter(&$tasks, $install_state) {
  $tasks['install_finished']['function'] = 'openscholar_install_finished';
  $tasks['install_finished']['display_name'] = t('Finished');
  $tasks['install_finished']['type'] = 'normal';
}

/**
 * Flavor selection form.
 */
function openscholar_flavor_form($form, &$form_state) {
  $options = array(
    'production' => t('Production Deployment'),
    'development' => t('Development'),
  );

  $form['os_profile_flavor'] = array(
    '#title' => t('Select a flavor'),
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => 'development'
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}


/**
 * Install type selection form
 */
function openscholar_install_type($form, &$form_state) {
  $options = array(
    'novsite' => t('Single site install'),
    'vsite' => t('Multi-tenant install'),
  );

  $form['os_profile_type'] = array(
    '#title' => t('Installation type'),
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => 'vsite',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}


/**
 * Form submit handler when selecting an installation type
 */
function openscholar_flavor_form_submit($form, &$form_state) {
  //Save the chosen flavor
  variable_set('os_profile_flavor', $form_state['input']['os_profile_flavor']);
}


/**
 * Form submit handler when selecting an installation type
 */
function openscholar_install_type_submit($form, &$form_state) {
  if(in_array($form_state['input']['os_profile_type'], array('vsite','single-tenant'))){
    variable_set('os_profile_type', $form_state['input']['os_profile_type']);
  }
}



function openscholar_vsite_modules_batch(&$install_state){
  //@todo this should be in an .inc file or something.
  $modules = array();
  $profile = drupal_get_profile();

  if(variable_get('os_profile_type', false) == 'vsite'){
    $data = file_get_contents("profiles/$profile/$profile.vsite.inc");
    $info = drupal_parse_info_format($data);
    if(is_array($info['dependencies'])){
      $modules = array_merge($modules,$info['dependencies']);
    }
  }

  if(variable_get('os_profile_flavor', false) == 'development'){
    $data = file_get_contents("profiles/$profile/$profile.development.inc");
    $info = drupal_parse_info_format($data);
    if(is_array($info['dependencies'])){
      $modules = array_merge($modules,$info['dependencies']);
    }
  }

  return _opnescholar_module_batch($modules);
}

/**
 * Set the private file system.
 */
function openscholar_private_file_system($form, &$form_state) {
  $form['private_directory'] = array(
    '#title' => st('Private files directory'),
    '#description' => st('An existing local file system path for storing private files. It should be writable by Drupal and not accessible over the web.'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Validate handler - verify that the private files directory is writeable and
 * exists.
 */
function openscholar_private_file_system_validate($form, &$form_state) {
  system_check_directory($form['private_directory']);
}

/**
 * Submit handler - save the private file directory path.
 */
function openscholar_private_file_system_submit($form, $form_state) {
  variable_set('file_private_path', $form_state['values']['private_directory']);
}

/**
 * Returns a batch operation definition that will install some $modules
 *
 * @param $modules
 *   An array of names of modules to install
 *
 * $return
 *   A batch definition.
 *
 * @see
 *   http://api.drupal.org/api/drupal/includes%21install.core.inc/function/install_profile_modules/7
 */
function _opnescholar_module_batch($modules) {
  $t = get_t();

  $files = system_rebuild_module_data();

  // Always install required modules first. Respect the dependencies between
  // the modules.
  $required = array();
  $non_required = array();

  // Add modules that other modules depend on.
  foreach ( $modules as $key => $module ) {
    if (isset($files[$module]) && $files[$module]->requires) {
      $modules = array_merge($modules, array_keys($files[$module]->requires));
    }
  }
  $modules = array_unique($modules);
  foreach ( $modules as $module ) {
    if (! empty($files[$module]->info['required'])) {
      $required[$module] = $files[$module]->sort;
    }
    else {
      $non_required[$module] = $files[$module]->sort;
    }
  }
  arsort($required);
  arsort($non_required);

  $operations = array();
  foreach ( $required + $non_required as $module => $weight ) {
    if (isset($files[$module])) {
      $operations[] = array('_install_module_batch',
        array(
          $module,
          $files[$module]->info['name']
        )
      );
    }
  }

  $additions = "";
  if(variable_get('os_profile_type', false) == 'vsite'){
    $additions .= "Multi-Tenant";
  }

  if(variable_get('os_profile_flavor', false) == 'development'){
    if(strlen($additions)){
      $additions .= " and ";
    }
    $additions .= "Development";
  }

  $batch = array(
    'operations' => $operations,
    'title' => st('Installing @needed modules.', array('@needed' => $additions)),
    'error_message' => st('The installation has encountered an error.'),
    'finished' => '_install_profile_modules_finished'
  );
  return $batch;
}

/**
 * Implements hook_form_FORM_ID_alter().
 **/
function openscholar_form_install_configure_form_alter(&$form, $form_state) {
  // Pre-populate the site name with the server name.
  $form['site_information']['site_name']['#default_value'] = $_SERVER['SERVER_NAME'];
}

function openscholar_install_finished(&$install_state) {
  drupal_set_title(st('Openscholar installation complete'));
  $messages = drupal_get_messages();
  $output = '<p>' . st('Congratulations, you\'ve successfully installed Openscholar!') . '</p>';
  if (isset($messages['error'])) {
    $output .= '<p>' . st('Review the messages above before visiting <a href="@url">your new site</a> or <a href="@settings" class="overlay-exclude">change Openscholar settings</a>.', array('@url' => url(''), '@settings' => url('admin/config/openscholar', array('query' => array('destination' => ''))))) . '</p>';
  }
  else {
    $output .= '<p>'. st('<a href="@url">Visit your new site</a> or <a href="@settings" class="overlay-exclude">change Openscholar settings</a>.', array('@url' => url(''), '@settings' => url('admin/config/openscholar', array('query' => array('destination' => ''))))) . '</p>';
  }

  // Flush all caches to ensure that any full bootstraps during the installer
  // do not leave stale cached data, and that any content types or other items
  // registered by the install profile are registered correctly.
  drupal_flush_all_caches();

  // Remember the profile which was used.
  variable_set('install_profile', drupal_get_profile());

  // Install profiles are always loaded last
  db_update('system')
    ->fields(array('weight' => 1000))
    ->condition('type', 'module')
    ->condition('name', drupal_get_profile())
    ->execute();

  // Cache a fully-built schema.
  drupal_get_schema(NULL, TRUE);

  // Run cron to populate update status tables (if available) so that users
  // will be warned if they've installed an out of date Drupal version.
  // Will also trigger indexing of profile-supplied content or feeds.
  drupal_cron_run();

  return $output;
}
