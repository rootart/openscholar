<?php
/**
 * @file
 * vsite.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function vsite_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'os_sitewide_front';
  $context->description = 'Active on the frontpage in multi-tenant installs';
  $context->tag = 'OpenScholar';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        '<front>' => '<front>',
      ),
    ),
    'os_site' => array(
      'values' => array(
        0 => '0',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'os_search_solr-site-search' => array(
          'module' => 'os_search_solr',
          'delta' => 'site-search',
          'region' => 'header_third',
          'weight' => '-10',
        ),
        'os_search_db-site-search' => array(
          'module' => 'os_search_db',
          'delta' => 'site-search',
          'region' => 'header_third',
          'weight' => '-10',
        ),
        'views-os_websites-block' => array(
          'module' => 'views',
          'delta' => 'os_websites-block',
          'region' => 'sidebar_first',
          'weight' => '-9',
        ),
        'vsite_register-get_your_site_block' => array(
          'module' => 'vsite_register',
          'delta' => 'get_your_site_block',
          'region' => 'sidebar_second',
          'weight' => '-8',
        ),
        'os-powered-by' => array(
          'module' => 'os',
          'delta' => 'powered-by',
          'region' => 'footer',
          'weight' => '10',
        ),
        'os-login' => array(
          'module' => 'os',
          'delta' => 'login',
          'region' => 'footer',
          'weight' => '-10',
        ),
        'views-os_activity-block_1' => array(
          'module' => 'views',
          'delta' => 'os_activity-block_1',
          'region' => 'footer',
          'weight' => '-30',
        ),
      ),
    ),
  );
  $context->condition_mode = 1;

  // Translatables
  // Included for use with string extractors like potx.
  t('Active on the frontpage in multi-tenant installs');
  t('OpenScholar');
  $export['os_sitewide_front'] = $context;

  $context = new stdClass();
  $context->disabled = FALSE; /*
                               * Edit this to true to make a default context
                               * disabled initially
                               */
  $context->api_version = 3;
  $context->name = 'os_users';
  $context->description = 'Context that is active when in a user area of an Openscholar site, like user profile or log in screen';
  $context->tag = 'OpenScholar';
  $context->conditions = array(
      'path' => array(
          'values' => array(
              	'user' => 'user',
				'users' => 'users',
              	'user/*' => 'user/*',
          )
      )
  );
  $context->reactions = array(
      'block' => array(
          'blocks' => array(
              'os-powered-by' => array(
                  'module' => 'os',
                  'delta' => 'powered-by',
                  'region' => 'footer',
                  'weight' => '10',
              ),
          )
      )
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('Context that is active when in a user area of an Openscholar site, like user profile or log in screen');
  t('OpenScholar');
  $export['os_users'] = $context;


  return $export;
}
