<?php

/**
 * @file
 * Admin page callbacks for the front page module.
 */

/**
 * Form for configuring front page settings.
 */
function front_admin($form, &$form_state) {

  $form['front_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Front Page Override'),
    '#description' => t('Enable this if you want the front page module to manage the home page.'),
    '#default_value' => config_get('front.settings', 'front_enable'),
  );

  // Load any existing settings and build the by redirect by role form
  $form['roles'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Roles'),
    '#description' => t('These are the settings for each role. If Front Page Override is enabled when a user reaches the home page the site will iterate through the roles below from top to bottom until firstly the user has the role and secondly the role is not set to SKIP. If no roles get selected the site front page will be shown. To rearrange the order in which the roles are processed you may do this at the !link.', array('!link' => l(t('Arrange tab'), 'admin/config/front/arrange'))),
    '#collapsible' => FALSE,
  );

  // build the form for roles
  $roles = user_roles();
  $front_data = front_get_all();

  // Map the available modes common for all roles.
  $modes = array(
    '' => t('Skip'),
    'themed' => t('Themed'),
    'full' => t('Full'),
    'redirect' => t('Redirect'),
    'alias' => t('Alias'),
  );

  // Set the description common for all roles.
  $descriptions = array(
    '' => t('Disable functionality for this role.'),
    'themed' => t('Means your default layout, theme and stylesheet will be loaded with your custom front.'),
    'full' => t('Allows you to display a front page with a completely different layout, style sheet, etc.'),
    'redirect' => t('Will automatically redirect visitors to a specific page specified in the REDIRECT TO box.'),
    'alias' => t('Will display the page listed in path as if it were the home page. This option does not redirect.'),
  );

  // Format the options to use as radio buttons
  $options = array();
  foreach ($modes as $key => $mode) {
    $options[$key] = "<strong>{$mode}:</strong> {$descriptions[$key]}";
  }

  // Iterate each role
  foreach ($roles as $rid => $role_label) {
    
    // Determine the mode for this role
    $mode = isset($front_data[$rid]['mode']) ? $front_data[$rid]['mode'] : '';

    $form['roles'][$rid] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Front Page for !rolename (%mode)', array('!rolename' => $role_label, '%mode' => $modes[$mode])),
      '#weight' => isset($front_data[$rid]['weight']) ? $front_data[$rid]['weight'] : 0,
    );

    $form['roles'][$rid]['mode'] = array(
      '#type' => 'radios',
      '#title' => t('Select mode'),
      '#default_value' => $mode,
      '#options' => $options,
    );

    // We need a wrapper because of a core bug that prevents the visible
    // #state from properly hiding text_format widgets:
    // @see: https://drupal.org/node/997826
    $form['roles'][$rid]['data_wrapper'] = array(
      '#type' => 'container',
      '#states' => array(
        'visible' => array(
          ':input[name="roles[' . $rid . '][mode]"]' => array(
            array('value' => 'full'),
            array('value' => 'themed'),
          ),
        ),
      ),
    );

    $form['roles'][$rid]['data_wrapper']['data'] = array(
      '#type' => 'text_format',
      '#title' => t('Data'),
	  '#rows' => 10,
      '#default_value' => (isset($front_data[$rid]['data']) && isset($front_data[$rid]['mode']) && ($front_data[$rid]['mode'] == 'themed' || $front_data[$rid]['mode'] == 'full')) ? $front_data[$rid]['data'] : NULL,
      '#format' => !empty($front_data[$rid]['filter_format']) ? $front_data[$rid]['filter_format'] : NULL,
      '#description' => t('Paste your HTML or TEXT here.') . '<br><strong>' . t('Note if you selected "Full" mode above:') . '</strong> ' . t('The "Full" mode will not  
	    display any of your existing theme. Instead, you must enter the full HTML code for a complete front page, including styles and/or javascript in the HEAD of the document, meta tags, etc. If this is your intention, you should also select a text format which does not apply any HTML filters, as that may strip out styles, HEAD tags, etc. You may need to create a new text format for this purpose.'),
    );

    $form['roles'][$rid]['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#default_value' => (isset($front_data[$rid]['data']) && isset($front_data[$rid]['mode']) && ($front_data[$rid]['mode'] == 'redirect' || $front_data[$rid]['mode'] == 'alias')) ? $front_data[$rid]['data'] : NULL,
      '#cols' => 20,
      '#rows' => 1,
      '#description' => t('If you are using <strong>Redirect</strong> or <strong>Alias</strong> you need to specify the path. An alias path should only include the URL part of a URL (eg "node/51"). A redirect path can contain a full URL including get parameters and fragment string (eg "node/51?page=5#anchor").'),
      '#states' => array(
        'visible' => array(
          ':input[name="roles[' . $rid . '][mode]"]' => array(
            array('value' => 'redirect'),
            array('value' => 'alias'),
          ),
        ),
      ),
    );
  }

  // if the token module exists then provide global token support
  if (module_exists('token')) {
    $form['token_help'] = array(
      '#theme' => 'token_tree',
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );

  return $form;
}

/**
 * Validation hook for front_admin.
 */
function front_admin_validate($form, &$form_state) {
  if (is_array($form_state['values']['roles'])) {
    foreach ($form_state['values']['roles'] as $rid => $role) {
      switch ($role['mode']) {
        case 'themed':
        case 'full':
          if (empty($role['data_wrapper']['data']['value'])) {
            form_set_error('roles][' . $rid . '][data][value', 'You must set the data field for ' . $role['mode'] . ' mode.');
          }
          break;
        case 'redirect':
          if (empty($role['path'])) {
            form_set_error('roles][' . $rid . '][path', 'You must set the path field for redirect mode.');
          }
          break;
        case 'alias':
          if (empty($role['path'])) {
            form_set_error('roles][' . $rid . '][path', 'You must set the path field for alias mode.');
          }
          elseif (!preg_match('@^[^?#]+$@', $role['path'])) {
            form_set_error('roles][' . $rid . '][path', 'You must set only the URI part of a URL in alias mode.');
          }
          break;
      }
    }
  }
}

/**
 * Submit hook for front_admin.
 */
function front_admin_submit($form, &$form_state) {
  config_set('front.settings', 'front_enable', $form_state['values']['front_enable']);

  if (is_array($form_state['values']['roles'])) {
    foreach ($form_state['values']['roles'] as $rid => $role) {
      switch ($role['mode']) {
        case 'themed':
        case 'full':
          $data = _front_get_by_rid_with_default($rid);

          $data['mode'] = $role['mode'];
          $data['data'] = $role['data_wrapper']['data']['value'];
          $data['filter_format'] = $role['data_wrapper']['data']['format'];

          config_set('front.settings', "front_role_{$rid}", $data);
          break;
        case 'redirect':
        case 'alias':
          $data = _front_get_by_rid_with_default($rid);

          $data['mode'] = $role['mode'];
          $data['data'] = $role['path'];
          $data['filter_format'] = '';

          config_set('front.settings', "front_role_{$rid}", $data);
          break;
        default:
          $data = _front_get_by_rid_with_default($rid);

          $data['mode'] = '';
          $data['data'] = '';
          $data['filter_format'] = '';

          config_set('front.settings', "front_role_{$rid}", $data);
          break;
      }
    }
  }
  backdrop_set_message(t('Your settings have been saved.'));
}

/**
 * Form for arranging the roles according to what order the roles should be processed.
 */
function front_admin_arrange_form($form, &$form_state) {
  $roles = user_roles();
  $front_data = front_get_all();
  foreach ($roles as $rid => $role) {
    $front_data[$rid]['name'] = $role;
  }

  $form['roles'] = array(
    '#tree' => TRUE,
  );
  foreach ($front_data as $role_id => $role) {
    $form['roles'][$role_id]['title']['#markup'] = $role['name'];
    $form['roles'][$role_id]['mode']['#markup'] = !empty($role['mode']) ? $role['mode'] : 'skip';
    $form['roles'][$role_id]['preview']['#markup'] = !empty($role['mode'])?l(t('preview'), 'front/preview/' . $role_id, array('attributes' => array('target' => '_blank'))) : '';
    if (!empty($role['mode'])) {
      $form['roles'][$role_id]['enabled'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable'),
        '#title_display' => 'invisible',
        '#default_value' => TRUE,
      );
    }
    else {
      $form['roles'][$role_id]['enabled']['#markup'] = 'disabled';
    }
    $form['roles'][$role_id]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#title_display' => 'invisible',
      '#delta' => 10,
      '#default_value' => isset($role['weight']) ? $role['weight'] : 0,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Order'),
  );
  return $form;
}

/**
 * Submit hook for front_admin_arrange_form.
 */
function front_admin_arrange_form_submit($form, &$form_state) {
  $front_data = front_get_all();
  foreach ($form_state['values']['roles'] as $rid => $role) {
    if (isset($role['mode']) && !$role['mode'] || !isset($front_data[$rid])) {
      $data = _front_get_by_rid_with_default($rid);

      $data['mode'] = '';
      $data['data'] = '';
      $data['filter_format'] = '';
      $data['weight'] = $role['weight'];

      config_set('front.settings', "front_role_{$rid}", $data);
    }
    else {
      $data = _front_get_by_rid_with_default($rid);

      $data['weight'] = $role['weight'];

      config_set('front.settings', "front_role_{$rid}", $data);
    }
  }
}

/**
 * Form for setting the <front> placeholder to another path.
 */
function front_admin_home_links($form, &$form_state) {
  $config = config('front.settings');
  $form['#config'] = 'front.settings';
  $form['front_home_link_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect your site HOME links to'),
    '#default_value' => $config->get('front_home_link_path'),
    '#cols' => 20,
    '#rows' => 1,
    '#description' => t('Specify where the user should be redirected to. An example would be <em>node/12</em>. Leave blank when you\'re not using HOME redirect.'),
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (config_get('system.core', 'clean_url') ? '' : '?q='),
  );

  return system_settings_form($form);
}

/**
 * Returns HTML for the front page arrange form into a table.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 */
function theme_front_admin_arrange_form($variables) {
  $form = $variables['form'];

  backdrop_add_tabledrag('front-page-arrange', 'order', 'sibling', 'front-page-weight');

  $header = array(
    t('Role'),
    t('Mode'),
    t('Preview'),
    t('Enabled'),
    t('Weight'),
  );

  $rows = array();
  foreach (element_children($form['roles']) as $rid) {
    $element = &$form['roles'][$rid];

    // Add special classes to be used for tabledrag.js.
    $element['weight']['#attributes']['class'] = array('front-page-weight');

    $row = array();
    $row[] = backdrop_render($element['title']);
    $row[] = backdrop_render($element['mode']);
    $row[] = backdrop_render($element['preview']);
    $row[] = backdrop_render($element['enabled']);
    $row[] = backdrop_render($element['weight']);

    $row = array_merge(array('data' => $row), $element['#attributes']);
    $row['class'][] = 'draggable';
    $rows[] = $row;
  }
  $output = '';
  if (empty($rows)) {
    $rows[] = array(array(
        'data' => 'no roles',
        'colspan' => '5',
      ));
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'front-page-arrange')));
  $output .= backdrop_render_children($form);
  return $output;
}
