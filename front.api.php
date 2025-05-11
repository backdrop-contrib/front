<?php
/**
 * @file
 * It has all the lists of the hook documentation
 * that other module can implement.
 */

/**
 * Hook to alert the front page data before setting is triggered.
 *
 * If there is any custom logic, this can be used
 * to have custom redirec path.
 *
 * This hook gets invoked only when there is valid entry in the
 * admin settings page. If 'skip' is set as option for any
 * role, then this won't get invoked.
 */
function hook_front_alter(&$front_page) {
  global $user;
  if ($front_page['mode'] == 'redirect'
      && in_array('customer user', array_values($user->roles))
      && $user->login_page == 'reports') {
    $front_page['path'] = 'user/reports';
  }
}
