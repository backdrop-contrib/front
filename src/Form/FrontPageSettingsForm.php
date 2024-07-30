<?php

namespace Drupal\front_page\Form;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\token\Token;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Configure site information settings for this site.
 */
class FrontPageSettingsForm extends ConfigFormBase {

  /**
   * The token service.
   *
   * @var \Drupal\token\Token
   */
  protected $tokenService;

  /**
   * The config factory service.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The current user.
   *
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected $currentUser;

  /**
   * The entity type manager service.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The module handler service.
   *
   * @var \Drupal\Core\Extension\ModuleHandlerInterface
   */
  protected $moduleHandler;

  /**
   * {@inheritdoc}
   */
  public function __construct(Token $token_service, ConfigFactoryInterface $config_factory, AccountInterface $current_user, EntityTypeManagerInterface $entity_type_manager, ModuleHandlerInterface $module_handler) {
    $this->tokenService = $token_service;
    $this->configFactory = $config_factory;
    $this->currentUser = $current_user;
    $this->entityTypeManager = $entity_type_manager;
    $this->moduleHandler = $module_handler;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('token'),
      $container->get('config.factory'),
      $container->get('current_user'),
      $container->get('entity_type.manager'),
      $container->get('module_handler')
    );
  }

  /**
   * Implements \Drupal\Core\Form\FormInterface::getFormId().
   */
  public function getFormId() {
    return 'front_page_admin';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    $config = $this->configFactory->get('front_page.settings');

    $form['front_page_enable'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Enable Front page override'),
      '#description' => $this->t('Enable this if you want the front page module to manage the home page.'),
      '#default_value' => $config->get('enabled') ?: FALSE,
    ];

    $form['disable_for_administrators'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Disable front page redirects for the administrator role.'),
      '#description' => $this->t('If checked, admin users will never be redirected, even if the authenticated user role has a redirect enabled.'),
      '#default_value' => $config->get('disable_for_administrators') ?: FALSE,
    ];

    // Load any existing settings and build the by redirect by role form.
    $form['roles'] = [
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => $this->t('Roles'),
    ];

    // Build the form for roles.
    $roles = $this->entityTypeManager->getStorage('user_role')->loadMultiple();

    // Iterate each role.
    foreach ($roles as $rid => $role) {

      $role_config = $config->get('roles.' . $rid);
      $form['roles'][$rid] = [
        '#type' => 'details',
        '#open' => FALSE,
        '#title' => $this->t('Front page for @rolename', ['@rolename' => $role->label()]),
      ];

      $form['roles'][$rid]['enabled'] = [
        '#type' => 'checkbox',
        '#title' => $this->t('Enable'),
        '#default_value' => $role_config['enabled'] ?? FALSE,
      ];

      $form['roles'][$rid]['weight'] = [
        '#type' => 'number',
        '#title' => $this->t('Weight'),
        '#default_value' => $role_config['weight'] ?? 0,
      ];

      $form['roles'][$rid]['path'] = [
        '#type' => 'textfield',
        '#title' => $this->t('Path'),
        '#default_value' => $role_config['path'] ?? '',
        '#cols' => 20,
        '#rows' => 1,
        '#description' => $this->t('A redirect path can contain a full URL including get parameters and fragment string (eg "/node/51?page=5#anchor"). Use tokens like [node:title].'),
        '#element_validate' => ['token_element_validate'],
      ];

      // Add the token tree help.
      if ($this->moduleHandler->moduleExists('token')) {
        $form['roles'][$rid]['token_help'] = [
          '#theme' => 'token_tree_link',
          '#token_types' => ['all'],
          '#show_restricted' => TRUE,
          '#global_types' => TRUE,
          '#click_insert' => TRUE,
          '#weight' => 90,
        ];
      }
    }

    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Save Settings'),
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $rolesList = $form_state->getUserInput()['roles'];
    if ($rolesList) {
      foreach ($rolesList as $rid => $role) {
        if (!empty($role['enabled']) && empty($role['path'])) {
          $form_state->setErrorByName('roles][' . $rid . '][path', $this->t('You must set the path field for redirect mode.'));
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->configFactory->getEditable('front_page.settings');

    // Set if all config are enabled or not.
    $config->set('enabled', $form_state->getValue('front_page_enable'));
    $config->set('disable_for_administrators', $form_state->getValue('disable_for_administrators'));

    // Set config by role.
    $rolesList = $form_state->getUserInput()['roles'];
    if (is_array($rolesList)) {
      foreach ($rolesList as $rid => $role) {
        // Replace tokens in the path.
        if ($this->moduleHandler->moduleExists('token')) {
          $role['path'] = $this->replaceTokens($role['path']);
        }
        $config->set('roles.' . $rid, $role);
      }
    }

    $config->save();
    parent::submitForm($form, $form_state);
  }

  /**
   * Helper function to replace tokens.
   */
  protected function replaceTokens($text) {
    $data = ['user' => $this->currentUser];
    $options = ['clear' => TRUE];
    return $this->tokenService->replace($text, $data, $options);
  }

}
