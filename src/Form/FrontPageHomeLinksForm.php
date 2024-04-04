<?php

namespace Drupal\front\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Configure site information settings for this site.
 */
class FrontPageHomeLinksForm extends ConfigFormBase {

  /**
   * Implements \Drupal\Core\Form\FormInterface::getFormID().
   */
  public function getFormID() {
    return 'front_admin_home_links';
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
    global $base_url;

    $config = $this->config('front.settings');
    $form['front_home_link_path'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Redirect your site HOME links to'),
      '#default_value' => $config->get('home_link_path'),
      '#cols' => 20,
      '#rows' => 1,
      '#description' => $this->t("Specify where the user should be redirected to. An example would be <em>/node/12</em>. Leave blank when you're not using HOME redirect."),
      '#field_prefix' => $base_url,
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $this->configFactory()->getEditable('front.settings')
      ->set('home_link_path', $form_state->getValue('front_home_link_path'))
      ->save();
    parent::submitForm($form, $form_state);
  }

}
