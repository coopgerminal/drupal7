<?php

/**
 * @file
 * Store administration menu items.
 */

/**
 * Menu callback which provides the store administration overview page.
 */
function uc_store_admin() {
  module_load_include('inc', 'system', 'system.admin');
  // Check for status report errors.
  if (system_status(TRUE) && user_access('administer site configuration')) {
    drupal_set_message(t('One or more problems were detected with your Drupal installation. Check the <a href="@status">status report</a> for more information.', array('@status' => url('admin/reports/status'))), 'error');
  }
  $blocks = array();
  if ($admin = db_query("SELECT menu_name, mlid FROM {menu_links} WHERE link_path = 'admin/store' AND module = 'system'")->fetchAssoc()) {
    $result = db_query("
      SELECT m.*, ml.*
      FROM {menu_links} ml
      INNER JOIN {menu_router} m ON ml.router_path = m.path
      WHERE ml.link_path != 'admin/help' AND menu_name = :menu_name AND ml.plid = :mlid AND hidden = 0", $admin, array('fetch' => PDO::FETCH_ASSOC));
    foreach ($result as $item) {
      _menu_link_translate($item);
      if (!$item['access']) {
        continue;
      }
      // The link 'description' either derived from the hook_menu 'description'
      // or entered by the user via menu module is saved as the title attribute.
      if (!empty($item['localized_options']['attributes']['title'])) {
        $item['description'] = $item['localized_options']['attributes']['title'];
      }
      $block = $item;
      $block['content'] = theme('admin_block_content', array('content' => system_admin_menu_block($item)));
      if (!empty($block['content'])) {
        $block['show'] = TRUE;
      }

      // Prepare for sorting as in function _menu_tree_check_access().
      // The weight is offset so it is always positive, with a uniform 5-digits.
      $blocks[(50000 + $item['weight']) . ' ' . $item['title'] . ' ' . $item['mlid']] = $block;
    }
  }

  ksort($blocks);
  $build['blocks'] = array(
    '#theme' => 'admin_page',
    '#blocks' => $blocks,
  );

  if ($results = module_invoke_all('uc_store_status')) {
    foreach ($results as $message) {
      switch ($message['status']) {
        case 'warning': $icon = 'alert.gif'; break;
        case 'error':   $icon = 'error.gif'; break;
        default:        $icon = 'info.gif';
      }
      $icon = theme('image', array('path' => drupal_get_path('module', 'uc_store') . '/images/' . $icon));

      $rows[] = array(
        array('data' => $icon, 'class' => array('icon')),
        array('data' => $message['title'], 'class' => array('title')),
        array('data' => $message['desc'], 'class' => array('message')),
      );
    }

    $build['status'] = array(
      '#theme' => 'table',
      '#caption' => '<h2>' . t('Store status') . '</h2>',
      '#rows' => $rows,
      '#attributes' => array('class' => array('uc-store-status')),
    );
  }

  return $build;
}

/**
 * Displays main reports page.
 */
function uc_store_reports() {
  $menu = menu_get_item('admin/store/reports');
  $content = system_admin_menu_block($menu);

  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
    '#weight' => 5,
  );

  return $build;
}

/**
 * Displays store configuration page.
 */
function uc_store_configuration_page() {
  $menu = menu_get_item('admin/store/settings');
  $content = system_admin_menu_block($menu);

  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
  );

  return $build;
}

/**
 * Form to configure address fields.
 *
 * @see uc_store_address_fields_form_submit()
 * @see theme_uc_store_address_fields_form()
 * @ingroup forms
 */
function uc_store_address_fields_form($form, &$form_state) {
  $form['uc_address_fields']['#tree'] = TRUE;
  $form['uc_address_fields_required']['#tree'] = TRUE;
  $form['uc_address_fields_weight']['#tree'] = TRUE;

  $fields = array(
    'first_name' => t('First name'),
    'last_name' => t('Last name'),
    'company' => t('Company'),
    'street1' => t('Street address 1'),
    'street2' => t('Street address 2'),
    'city' => t('City'),
    'zone' => t('State/Province'),
    'country' => t('Country'),
    'postal_code' => t('Postal code'),
    'phone' => t('Phone number'),
  );
  $current = variable_get('uc_address_fields', drupal_map_assoc(array('first_name', 'last_name', 'phone', 'company', 'street1', 'street2', 'city', 'zone', 'postal_code', 'country')));
  $required = variable_get('uc_address_fields_required', drupal_map_assoc(array('first_name', 'last_name', 'street1', 'city', 'zone', 'postal_code', 'country')));
  $weight = uc_store_address_field_weights();
  foreach ($fields as $field => $label) {
    $form['uc_address_fields'][$field] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($current[$field]) ? TRUE : FALSE,
    );
    $form['uc_address_fields_required'][$field] = array(
      '#type' => 'checkbox',
      '#default_value' => isset($required[$field]) ? TRUE : FALSE,
    );
    $form['uc_address_fields_weight'][$field] = array(
      '#type' => 'weight',
      '#default_value' => (isset($weight[$field])) ? $weight[$field] : 0,
      '#attributes' => array('class' => array('uc-store-address-fields-weight')),
    );
    $form['fields'][$field]['default'] = array(
      '#markup' => $label,
    );
    $form['fields'][$field]['uc_field_' . $field] = array(
      '#type' => 'textfield',
      '#default_value' => uc_get_field_name($field),
      '#size' => 32,
    );
    $form['fields'][$field]['#weight'] = (isset($weight[$field])) ? $weight[$field] : 99;
  }

  $form['#submit'][] = 'uc_store_address_fields_form_submit';
  if (function_exists('i18n_variable_form_alter_settings')) {
    if ($i18n_variables = i18n_variable_form_alter_settings($form, i18n_variable_list())) {
      $form['#submit'][] = 'i18n_variable_form_submit';
      $form['#i18n_variables'] = $i18n_variables;
      $form += i18n_variable_form_selector();
    }
  }
  $form = system_settings_form($form);
  unset($form['#theme']);

  return $form;
}

/**
 * Returns HTML for uc_store_address_fields_form().
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see uc_store_address_fields_form()
 * @ingroup themeable
 */
function theme_uc_store_address_fields_form($variables) {
  $form = $variables['form'];

  $header = array(t('Field'), t('Title'), t('Enabled'), t('Required'), t('List position'));

  // Sort fields by weight
  uasort($form['fields'], 'element_sort');

  foreach (element_children($form['fields']) as $field) {
    $row = array(
      drupal_render($form['fields'][$field]['default']),
      drupal_render($form['fields'][$field]['uc_field_' . $field]),
      drupal_render($form['uc_address_fields'][$field]),
      drupal_render($form['uc_address_fields_required'][$field]),
      drupal_render($form['uc_address_fields_weight'][$field]),
    );

    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  drupal_add_tabledrag('uc-store-address-fields-weight-table', 'order', 'sibling', 'uc-store-address-fields-weight');

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'uc-store-address-fields-weight-table'),
  ));
  $output .= drupal_render_children($form);

  return $output;
}

/**
 * Saves the address fields settings.
 *
 * @see uc_store_address_fields_form()
 */
function uc_store_address_fields_form_submit($form, &$form_state) {
  $form_state['values']['uc_address_fields'] = array_filter($form_state['values']['uc_address_fields']);
  $form_state['values']['uc_address_fields_required'] = array_filter($form_state['values']['uc_address_fields_required']);
}

/**
 * Form builder for store settings.
 *
 * @see uc_store_settings_form_validate()
 * @ingroup forms
 */
function uc_store_settings_form($form, &$form_state) {
  $form['store'] = array('#type' => 'vertical_tabs');

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic information'),
    '#group' => 'store',
  );
  $form['basic']['uc_store_name'] = uc_textfield(t('Store name'), uc_store_name(), FALSE, NULL, 64);
  $form['basic']['uc_store_owner'] = uc_textfield(t('Store owner'), variable_get('uc_store_owner', NULL), FALSE, NULL, 64);
  $form['basic']['uc_store_email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => NULL,
    '#size' => 32,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#default_value' => uc_store_email(),
  );
  $form['basic']['uc_store_email_include_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include the store name in the "From" line of store e-mails.'),
    '#description' => t('May not be available on all server configurations. Turn off if this causes problems.'),
    '#default_value' => variable_get('uc_store_email_include_name', TRUE),
  );
  $form['basic']['uc_store_phone'] = uc_textfield(t('Phone number'), variable_get('uc_store_phone', NULL), FALSE);
  $form['basic']['uc_store_fax'] = uc_textfield(t('Fax number'), variable_get('uc_store_fax', NULL), FALSE);
  $form['basic']['uc_store_help_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Store help page'),
    '#description' => t('The Drupal page for the store help link.'),
    '#default_value' => variable_get('uc_store_help_page', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );

  $form['address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Store address'),
    '#group' => 'store',
  );
  $form['address']['address'] = array(
    '#type' => 'uc_address',
    '#default_value' => array(
      'uc_store_street1' => variable_get('uc_store_street1', ''),
      'uc_store_street2' => variable_get('uc_store_street2', ''),
      'uc_store_city' => variable_get('uc_store_city', ''),
      'uc_store_zone' => variable_get('uc_store_zone', 0),
      'uc_store_country' => isset($form_state['values']) ? $form_state['values']['uc_store_country'] : uc_store_default_country(),
      'uc_store_postal_code' => variable_get('uc_store_postal_code', ''),
    ),
    '#required' => FALSE,
    '#key_prefix' => 'uc_store',
  );

  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency format'),
    '#group' => 'store',
  );
  $form['currency']['uc_currency_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Default currency'),
    '#description' => t('While not used directly in formatting, the currency code is used by other modules as the primary currency for your site.  Enter here your three character <a href="!url">ISO 4217</a> currency code.', array('!url' => 'http://en.wikipedia.org/wiki/ISO_4217#Active_codes')),
    '#default_value' => variable_get('uc_currency_code', 'USD'),
    '#maxlength' => 3,
    '#size' => 5,
  );
  $form['currency']['example'] = array(
    '#type' => 'textfield',
    '#title' => t('Current format'),
    '#value' => uc_currency_format(1000.1234),
    '#disabled' => TRUE,
    '#size' => 10,
  );
  $form['currency']['uc_currency_sign'] = uc_textfield(t('Currency Sign'), variable_get('uc_currency_sign', '$'), FALSE, NULL, 10, 10);
  $form['currency']['uc_sign_after_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display currency sign after amount.'),
    '#default_value' => variable_get('uc_sign_after_amount', FALSE),
  );
  $form['currency']['uc_currency_thou'] = uc_textfield(t('Thousands Marker'), variable_get('uc_currency_thou', ','), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_dec'] = uc_textfield(t('Decimal Marker'), variable_get('uc_currency_dec', '.'), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_prec'] = array(
    '#type' => 'select',
    '#title' => t('Number of decimal places'),
    '#options' => drupal_map_assoc(array(0, 1, 2)),
    '#default_value' => variable_get('uc_currency_prec', 2),
  );

  $form['weight'] = array(
    '#type' => 'fieldset',
    '#title' => t('Weight format'),
    '#description' => t('Supply a format string for each unit. !value represents the weight value.'),
    '#group' => 'store',
  );
  $units = array(
    'lb' => t('Pounds'),
    'oz' => t('Ounces'),
    'kg' => t('Kilograms'),
    'g' => t('Grams'),
  );
  $form['weight']['uc_weight_unit'] = array(
    '#type' => 'select',
    '#title' => t('Default unit of measurement'),
    '#default_value' => variable_get('uc_weight_unit', 'lb'),
    '#options' => $units,
  );
  foreach ($units as $unit => $name) {
    $form['weight']['uc_weight_format_' . $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#default_value' => variable_get('uc_weight_format_' . $unit, '!value ' . $unit),
    );
  }

  $form['length'] = array(
    '#type' => 'fieldset',
    '#title' => t('Length format'),
    '#description' => t('Supply a format string for each unit. !value represents the length value.'),
    '#group' => 'store',
  );
  $units = array(
    'in' => t('Inches'),
    'ft' => t('Feet'),
    'cm' => t('Centimeters'),
    'mm' => t('Millimeters'),
  );
  $form['length']['uc_length_unit'] = array(
    '#type' => 'select',
    '#title' => t('Default unit of measurement'),
    '#default_value' => variable_get('uc_length_unit', 'in'),
    '#options' => $units,
  );
  foreach ($units as $unit => $name) {
    $form['length']['uc_length_format_' . $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#default_value' => variable_get('uc_length_format_' . $unit, '!value ' . $unit),
    );
  }

  $form['display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#group' => 'store',
  );
  $form['display']['uc_customer_list_address'] = array(
    '#type' => 'radios',
    '#title' => t('Primary customer address'),
    '#description' => t('Select the address to be used on customer lists and summaries.'),
    '#options' => array(
      'billing' => t('Billing address'),
      'shipping' => t('Shipping address'),
    ),
    '#default_value' => variable_get('uc_customer_list_address', 'billing'),
  );
  $form['display']['uc_footer_message'] = array(
    '#type' => 'radios',
    '#title' => t('Footer message for store pages'),
    '#options' => array_merge(
      array(0 => t('Randomly select a message from the list below.')),
      _uc_store_footer_options(),
      array('none' => t('(Do not display a message in the footer.)'))
    ),
    '#default_value' => variable_get('uc_footer_message', 0),
    '#weight' => 10,
  );

  return system_settings_form($form);
}

/**
 * Validates store e-mail address for uc_store_settings_form().
 *
 * @see uc_store_settings_form()
 */
function uc_store_settings_form_validate($form, &$form_state) {
  $mail = trim($form_state['values']['uc_store_email']);
  if (!valid_email_address($mail)) {
    form_set_error('uc_store_email', t('The e-mail address %mail is not valid.', array('%mail' => $mail)));
  }
}

/**
 * Implements hook_uc_store_status().
 */
function uc_store_uc_store_status() {
  $messages = array();

  // Check to see if there are any updated CIF files that need to be installed.
  $countries = array();
  $result = db_query("SELECT * FROM {uc_countries}");
  foreach ($result as $country) {
    $countries[t($country->country_name)] = $country;
  }
  uksort($countries, 'strnatcasecmp');
  $files = _uc_country_import_list();

  $updates = array();
  if (is_array($countries)) {
    foreach ($countries as $country) {
      if ($country->version < $files[$country->country_id]['version'] && $country->version > 0) {
        $updates[] = $country->country_name;
      }
      unset($files[$country->country_id]);
    }
  }

  if (count($updates)) {
    $messages[] = array(
      'status' => 'warning',
      'title'  => t('Countries'),
      'desc'   => t('Updates are available for the following installed countries: %countries. You may update these countries on the <a href="@url">country settings</a> page.', array('%countries' => implode(', ', $updates), '@url' => url('admin/store/settings/countries'))),
    );
  }
  else {
    $messages[] = array(
      'status' => 'ok',
      'title'  => t('Countries'),
      'desc'   => t('Country definitions are up-to-date.'),
    );
  }

  // Check to see if the store e-mail address has been set.
  if (variable_get('uc_store_email', '') == '') {
    $messages[] = array(
      'status' => 'error',
      'title'  => t('Store settings'),
      'desc'   => t('Store e-mail address has not been set. Please enter it <a href="@url">here</a>.', array('@url' => url('admin/store/settings/store'))),
    );
  }
  else {
    $messages[] = array(
      'status' => 'ok',
      'title'  => t('Store settings'),
      'desc'   => t('Store e-mail address is set.'),
    );
  }

  return $messages;
}

/* start Germinal functions*/

function uc_store_comandes($message = NULL, $query = NULL, $count_query = NULL, $page_length = 25) {
// mostra les comandes segons el usuari que crida la funcio
// si no es admin, filtra per el centre del usuari

  // TODO: borrar de uc_socis els usuaris borrats de drupal
   $message = '';
   $header = array(
      t('Veure'),
      array('data' => t('Centre'), 'field' => 'c.name'),
     array('data' => t('Data'), 'field' => 'o.setmana_recollida', 'sort' => 'desc'),
      array('data' => t('Quantes'), 'field' => 'quants'),
    );
  if (is_null($query)) {
    $query = db_select('uc_orders', 'o')->extend('TableSort')->extend('PagerDefault');

     if (! germinal_is_admin()) {
      $centre_info = uc_store_get_informacio_centre_by_soci();
      $centre_filter = intval($centre_info->uid);
      $query->join('uc_centres','c','o.centre = c.uid AND o.centre = :centre', array(':centre' => $centre_filter )) ;
    }else{
      $query->join('uc_centres','c','o.centre = c.uid ') ;
    }

    $query->addExpression('COUNT(o.setmana_recollida)', 'quants');
    $query->fields('o',array( 'setmana_recollida','centre' ));
    $query->fields('c',array('name'));
    $query->groupBy('setmana_recollida')->groupBy('centre');
    $query->orderByHeader($header);
    $query->limit(25);

    $result = $query->execute();
  }


  $rows = array();
   foreach($result as $comandes ) {
    $icons = l(uc_store_get_icon('admin/store/socis', TRUE),
               'admin/store/reports/comanda_setmanal/'. $comandes->setmana_recollida .'/'.$comandes->centre,
               array('html' => TRUE, 'attributes' => array('title' => t("Veure informació de les comandes de la setmana i centre.")))
             );
    $setmana = ucfirst($comandes->setmana_recollida);
    $centre = ucfirst($comandes->name);
    $quants = ucfirst($comandes->quants);
    $rows[] = array(
   $icons ,
  check_plain($centre),
  check_plain($setmana),
        $quants);
  }

  $output =  theme_table(array('header'=>$header,
    'rows'=>$rows,
    'attributes'=>array(),
    'caption'=>'',
    'colgroups'=>array(),
    'sticky'=>'',
    'empty'=>''));

  $output .= theme('pager',array('tags' => array()));

  return $output;
}

function uc_store_get_centre_name($id) {
  if ($id == 0) {
    return "Sense assignar";
  }

  $sql = "SELECT c.name FROM {uc_centres} c WHERE uid = $id";
  $result = db_query($sql);
  $name = $result->fetchAssoc();
  return $name['name'];

}

function uc_store_comanda_setmanal( $query = NULL) {

  $setmana = arg(4);
  $centreid = intval(arg(5));
  $centre  = uc_store_get_centre_name($centreid);
  $missatge = "Informació de les comandes per la setmana " . $setmana . " del centre ".$centre."<br/>\n";

  $header = array(
    t('Veure'),
    array('data' => t('Nom'), 'field' => 'u.name'),
    array('data' => t('Núm. Soci'), 'field' => 's.numsoci', 'sort' => 'desc'),
    array('data' => t('Núm Productes'), 'field' => 'numproductes'),
    array('data' => t('Estat'), 'field' => 'o.order_status')
  );
  // aquesta query retorna els socis que han fet comanda una setmana en un centre i la quantitat de productes demanats,
  // encara que no hagin demanat cap producte, això permet detectar problemes
  /*$query =   "SELECT uc_socis.numsoci, users.name, uc_orders.order_id, COUNT(uc_order_products.order_id) AS numproductes, uc_orders.order_status" .
        "  FROM uc_socis, users, uc_orders " .
            " LEFT JOIN uc_order_products ON uc_orders.order_id = uc_order_products.order_id " .
        "  WHERE uc_orders.setmana_recollida='".$setmana."' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.centre = ".$centreid." GROUP BY uc_orders.order_id ";

*/

  $query = db_select('uc_socis', 's');
  $query->extend('TableSort')->orderByHeader($header);

  $query->innerJoin( 'users','u',' u.uid = s.uid ');
  $query->innerJoin( 'uc_orders','o', ' o.uid = s.uid ' );
  $query->leftJoin( 'uc_order_products','op','o.order_id = op.order_id ');
  $query->fields('s', array('numsoci'));
  $query->fields('u', array('name'));
  $query->fields('o', array('order_id','order_status'));
  $query->addExpression('COUNT(op.order_id)', 'numproductes');
  $query->where( "o.setmana_recollida = :setmana AND o.uid = s.uid AND o.uid = u.uid AND o.centre = :centreid" , array( ':setmana'=> $setmana , ':centreid'=> $centreid)  );
  $query->groupBy('order_id' );

  $result = $query->execute();
    $rows = array();
  global $base_url;

  foreach ( $result as $comandasoci ) {

    $icons = l(uc_store_get_icon('admin/store/orders', TRUE),
               'admin/store/orders/'. $comandasoci->order_id,
               array('html' => TRUE, 'attributes' => array('title' => t("View order.")))
             );
    $name = ucfirst($comandasoci->name);
    $numsoci = ucfirst($comandasoci->numsoci);
    $numproductes = ucfirst($comandasoci->numproductes);
    $estat = ucfirst($comandasoci->order_status);
    if (strcasecmp($estat, "pending") == 0) {
      $estat = "<img src=\"$base_url/misc/watchdog-ok.png\" /> Ok";
  } else if (strcasecmp($estat, "in_checkout") == 0) {
      $estat = "<img src=\"$base_url/misc/watchdog-error.png\" /> No acabada";
  } else if (strcasecmp($estat, "canceled") == 0) {
      $estat = "<img src=\"$base_url/misc/watchdog-warning.png\" /> Cancel·lada";
  }
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($name)),
        array('data' => $numsoci),
        array('data' => $numproductes),
        array('data' => $estat)),
    );
  }

$output =  '<p>'. $missatge .'</p>' .
"<a href=\"$base_url/admin/store/reports/informes/".$setmana."/".$centreid."\">Generar llistats per descarregar.</a>". theme('table',
    array('header'=> $header,
    'rows'=>$rows,
    'attributes'=>array(),
    'caption'=>'',
    'colgroups'=>array(),
    'sticky'=>TRUE,
    'empty'=>''))  .'<br />' ;

//$output .= "<a href=\"?q=admin/store/reports/informes/".$setmana."/".intval(arg(5))."\">Generar llistats</a>";
 //$output .= ;

  return $output;
}

//TODO: poder afegir informaciço extra:
// info del soci en el llistat del soci
// setmana a les altres
function convert_to_excel($centre_id, $centre, $setmana, $fulla, $array_data, $in_invert = false, $small_columns = false) {
  $dir = drupal_get_path('module', 'uc_store');

  //documentation on the spreadsheet package is at:
  //http://pear.php.net/manual/en/package.fileformats.spreadsheet-excel-writer.php

  $curr_dir = getcwd();
  chdir($dir .'/phpxls/');
  require_once('Writer.php');
  chdir($curr_dir);

  $export_filename = tempnam("tmp/" ,"germinal_");

  $export_file = "xlsfile:/".$export_filename;

  $workbook = new Spreadsheet_Excel_Writer($export_filename);
  $workbook->setVersion(8);

  $format_und =& $workbook->addFormat();
  $format_und->setTop(1);
  $format_und->setBottom(1);
  $format_und->setLeft(1);
  $format_und->setRight(1);
  $format_und->setBold();
  $format_und->setColor('black');
  $format_und->setFontFamily('Arial');
  $format_und->setSize(8);

  $format_codi =& $workbook->addFormat();
  $format_codi->setTop(1);
  $format_codi->setBottom(1);
  $format_codi->setLeft(1);
  $format_codi->setRight(1);
  $format_codi->setColor('black');
  $format_codi->setFontFamily('Arial');
  $format_codi->setSize(8);

  $format_reg =& $workbook->addFormat();
  $format_reg->setColor('black');
  $format_reg->setFontFamily('Arial');
  $format_reg->setSize(8);

  foreach ($array_data as $sheet_key => $sheet_array_data) {
    $worksheet =& $workbook->addWorksheet($sheet_key);

    $worksheet->setInputEncoding('UTF-8');

    $worksheet->setColumn(0,0, 20.0);//setColumn(startcol,endcol,float)

    $invert = $in_invert;
    if ($centre_id == CENTRE_SANTS) {
      if (strcmp($fulla, "graella") == 0) {
        if (strcmp($sheet_key, "FRUITA I VERDURA") == 0) {
          //$worksheet->repeatColumns(0);
        } elseif ((strcmp($sheet_key, "FLECA TASCÓ") == 0) || (strcmp($sheet_key, "LLET I DERIVATS") == 0)) {
          $invert = false;
        }
      }
    }


    $contingut_start = 1;
    // Si hi ha la columna de codi la fem més estreta
    if ((count($sheet_array_data) > 0) && (isset($sheet_array_data[0]["codi"]))) {
      $worksheet->setColumn(1,1, 6.0);
      $contingut_start = 2;
    }

    $worksheet->write(0, 0, $centre . " / " . $setmana . " / " . $fulla . " / " . $sheet_key, $format_reg);

    if ($invert) {
      $row_start_pos = 1;
      $col_start_pos = 2;
    } else {
      $row_start_pos = 3;
      $col_start_pos = 0;
    }

    $row_pos = $row_start_pos;
    $key_used_pos = $col_start_pos;
    $key_mappings = array();
    $keys = array();
    $key_mappings["producte"] = $key_used_pos++;
    foreach ($sheet_array_data as $row) {
      foreach ($row as $key=>$value) {
        if (strcmp($key, "producte") == 0) {
        } else if (strcmp($key, "codi") == 0) {
          if (! isset($key_mappings[$key])) {
            $key_mappings[$key] = $key_used_pos++;
          }
        } else {
          if (! in_array($key, $keys)) {
            $keys[] = $key;
          }
        }
      }
    }
    sort($keys);
    foreach ($keys as $key) {
      $key_mappings[$key] = $key_used_pos++;
    }

    foreach ($sheet_array_data as $row) {
      foreach ($row as $key=>$value) {
        $key_pos = $key_mappings[$key];

        $fmt  =& $format_reg;
        if ($key_pos==$col_start_pos) {
          $fmt =& $format_und;
        } else if (strcmp($key, "codi") == 0) {
          $fmt =& $format_codi;
        }

        if ($invert) {
          $worksheet->write($key_pos, $row_pos, $value, $fmt);
        } else {
          $worksheet->write($row_pos, $key_pos, $value, $fmt);
        }

      }
      $row_pos++;
    }

    // Mirem quantes columnes usem pel contingut (+codi)
    if ($invert) {
      $used_columns = $row_pos-1;
    } else {
      $used_columns = $key_used_pos-1;
    }

    // Un cop sabem quantes columnes tenim els hi posem una mida
    if ($small_columns) {
      $worksheet->setColumn($contingut_start,$used_columns, 6.00);
    } else {
      $worksheet->setColumn($contingut_start,$used_columns, 12.00);
    }

    foreach ($key_mappings as $key => $key_pos) {
      if ($invert) {
        $worksheet->write($key_pos, 0, $key, $format_und);
      } else {
        $worksheet->write(2, $key_pos, $key, $format_und);
      }
    }

    // Si és la graella de farró volen posar al final de tot el codi i el nom altre cop
    if (($centre_id == CENTRE_FARRO) && (strcmp($fulla, "graella") == 0)) {
      // Posem el títol de les columnes del final
      $worksheet->setColumn($key_used_pos,$key_used_pos, 6.0);//setColumn(startcol,endcol,float)
      $worksheet->write(2, $key_used_pos, "codi", $format_und);
      $worksheet->setColumn($key_used_pos+1,$key_used_pos+1, 20.0);//setColumn(startcol,endcol,float)
      $worksheet->write(2, $key_used_pos+1, "producte", $format_und);

      // Tornem a recorrer les dades i posem el codi i el producte a les noves columnes
      $row_pos = $row_start_pos;
      foreach ($sheet_array_data as $row) {
        $worksheet->write($row_pos, $key_used_pos, $row["codi"], $format_codi);
        $worksheet->write($row_pos, $key_used_pos+1, $row["producte"], $format_und);
        $row_pos++;
      }
    }
  }

  $workbook->close();

  return $export_filename;
}


function uc_store_format_numeros($valor) {
  return number_format($valor, 2, ',', '');
}

function uc_store_get_nom_separadors($centre) {
  // Seleccionem el nom dels separadors
  $sql =   "SELECT node.title AS nom, uc_products.ordering " .
        "FROM node, uc_products " .
        "WHERE node.nid = uc_products.nid AND uc_products.separador=1 AND uc_products.centre = ".$centre;

  $separadors = array();
  $result = db_query($sql);
  foreach($result as $data ) {
    $separadors[$data->ordering+1] = $data->nom;
  }
  return $separadors;
}


define('CENTRE_SANTS', 1);
define('CENTRE_FARRO', 2);
define('CENTRE_SARRIA', 4);
// Aquesta funció agafa la suma de cada producte dels demanats per un centre i setmana
function uc_store_informe_productes($setmana, $centre) {
  // Seleccionem la suma de les quantitats dels productes per una setmana de recollida i un centre concret
  $sql =   "SELECT uc_order_products.title AS producte, SUM(uc_order_products.qty) AS quantitat, uc_products.ordering, uc_products.codi " .
        "FROM uc_orders, uc_order_products, uc_products " .
        "WHERE uc_order_products.nid = uc_products.nid AND uc_orders.setmana_recollida='".$setmana."' AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_orders.order_status LIKE 'pending'".
        "GROUP BY uc_order_products.nid ".
        "ORDER BY uc_products.ordering ASC, uc_order_products.title ASC";

  // La capçalera de la taula només conté el producte i la quantitat
  $xsl = array();

  // Per cada producte posem el nom i la quantitat
    $result = db_query($sql);
    foreach ($result as $data ) {
    if (! isset($xsl[$data->ordering])) {
      $xsl[$data->ordering] = array();
    }
  $xsl_order = $xsl[$data->ordering];
   if ($centre == CENTRE_SANTS) {
      $xsl_order[] = array("producte" => $data->producte, "codi" => $data->codi, "1. q. demanada" => $data->quantitat, "2. q. arribada" => "" , "3. qualitat" => "");
    } else {
      $xsl_order[] = array("producte" => $data->producte, "codi" => $data->codi, "quantitat" => $data->quantitat);
    }
    $xsl[$data->ordering] = $xsl_order;

    }

  // Canviem les claus de l'array del xsl de la posicio al nom del separador
  $separadors = uc_store_get_nom_separadors($centre);
  foreach ($separadors as $pos => $separador) {
    if (isset($xsl[$pos])) {
      $xsl[$separador] = $xsl[$pos];
      unset($xsl[$pos]);
    }
  }

  return $xsl;
}



// Aquesta funció agafa la graella de productes/soci per a preparar la comanda
function uc_store_informe_productes_soci($setmana, $centre) {
  // Seleccionem per cada soci d'un centre els productes demanats per una setmana de recollida
  // el resultat és redundant, però així fem només una consulta
  $sql =   "SELECT uc_socis.numsoci, users.name, uc_orders.order_id, uc_order_products.title, uc_order_products.qty, uc_products.codi, uc_products.ordering " .
      "FROM uc_socis, users, uc_orders, uc_order_products, uc_products " .
      "WHERE uc_orders.setmana_recollida='$setmana' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_products.nid = uc_order_products.nid AND uc_orders.order_status LIKE 'pending' ".
      "ORDER BY uc_products.ordering ASC, uc_order_products.title ASC";

  // El primer que fem es recorrer el resultat i crear un llistat de productes i per cada soci els productes que ha demanat
  $socis = array();
  $productes = array();
  $soci = array();
  $result = db_query($sql);
  foreach ($result as $data ) {
    $numsoci = $data->numsoci;
    $nomsoci = $data->name;
    $nomproducte = $data->title;
    $codiproducte = $data->codi;
      $quantproducte = $data->qty;
      $ordering = $data->ordering;

      if (!isset($productes[$ordering])) {
        $productes[$ordering] = array();
    }
    $productes_ordering = $productes[$ordering];
      $productes_ordering[$codiproducte] = $nomproducte;
       $productes[$ordering] = $productes_ordering;


    if (!in_array( $numsoci , $socis)) {
        $soci = array();
        $soci['nom'] = $nomsoci;
        $soci['num'] = $numsoci;
    }
    $soci[$codiproducte] = $quantproducte;
    $socis[$numsoci] = $soci;
    }

  ksort($socis);

  $xls = array();

  foreach ($productes as $ordering => $productes_ordering) {
    $xls_ordering = array();
    foreach ($productes_ordering as $codiproducte => $nomproducte) {
      $xls_array = array();
      if (($centre == CENTRE_FARRO) || ($centre == CENTRE_SARRIA)) {
        $xls_array["producte"] = $nomproducte;
        $xls_array["codi"] = $codiproducte;
      } else {
        $xls_array["producte"] = $nomproducte." (".$codiproducte.")";
      }
      foreach ($socis as $idsoci => $soci) {
        if (isset($soci[$codiproducte])) {
          if (($centre == CENTRE_FARRO) || ($centre == CENTRE_SARRIA)) {
            $xls_array[$soci['num']] = $soci[$codiproducte];
          } else {
            $fnum = sprintf("%03d",$soci['num']);
            $xls_array[$fnum." - ".$soci['nom']] = $soci[$codiproducte];
          }
        }
      }
      $xls_ordering[] = $xls_array;
    }
    $xls[$ordering] = $xls_ordering;
  }

  // Canviem les claus de l'array del xsl de la posicio al nom del separador
  $separadors = uc_store_get_nom_separadors($centre);
  foreach ($separadors as $pos => $separador) {
    if (isset($xls[$pos])) {
      $xls[$separador] = $xls[$pos];
      unset($xls[$pos]);
    }
  }

    return  $xls;
}


// Aquesta funció agafa la llista de productes demanats per cada soci
function uc_store_informe_socis($setmana, $centre) {
  // Seleccionem per cada soci d'un centre els productes demanats per una setmana de recollida
  // el resultat és redundant, però així fem només una consulta
  $sql =   "SELECT uc_socis.numsoci, users.name, uc_orders.order_id, uc_order_products.title, uc_order_products.qty, uc_products.codi, uc_products.ordering " .
      "FROM uc_socis, users, uc_orders, uc_order_products, uc_products " .
      "WHERE uc_orders.setmana_recollida='$setmana' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_products.nid = uc_order_products.nid AND uc_orders.order_status LIKE 'pending' ".
      "ORDER BY uc_socis.numsoci ASC, uc_products.ordering ASC, uc_order_products.title ASC";

  // El primer que fem es recorrer el resultat i crear un llistat de productes i per cada soci els productes que ha demanat
  $socis = array();
  $productes = array();

    $result = db_query($sql);
    foreach ($result as $data ) {
      $numsoci = $data->numsoci;
      $nomsoci = $data->name;
      $nomproducte = $data->title;
    $codiproducte = $data->codi;
      $quantproducte = $data->qty;
      $ordering = $data->ordering;

      if (!isset($productes[$ordering])) {
        $productes[$ordering] = array();
    }
    $productes_ordering = $productes[$ordering];
      $productes_ordering[$codiproducte] = $nomproducte;
       $productes[$ordering] = $productes_ordering;


      if (!in_array($numsoci."-".$nomsoci,$socis) ) {
        $soci = array();
        $soci['nom'] = $nomsoci;
        $soci['num'] = $numsoci;
    }
    $soci[$codiproducte] = $quantproducte;
    $socis[$numsoci."-".$nomsoci] = $soci;
    }


  $separadors = uc_store_get_nom_separadors($centre);

  $xls = array();
  foreach ($socis as $idsoci => $soci) {
    $xls_soci = array();
    foreach ($productes as $ordering => $productes_ordering) {
      $xls_soci_ordering = array();
      foreach ($productes_ordering as $codiproducte => $nomproducte) {
        $xls_array = array();
        $xls_array["producte"] = $nomproducte." (".$codiproducte.")";
        $xls_ordering[] = $xls_array;

        if (isset($soci[$codiproducte])) {
          $productesoci = $soci[$codiproducte];
          if ($centre == CENTRE_SANTS) { // a sants afegim un espai davant dels codis de producte que comencen per 0, sinó se'l menja
            if ((is_string($codiproducte)) && (substr($codiproducte, 0 ,1) == "0")) {
              $scodiproducte = " " . $codiproducte;
            } else {
              $scodiproducte = $codiproducte;
            }
            $xls_soci_ordering[] = array("producte" => $nomproducte, "codi" => $scodiproducte, "quantitat" => $productesoci);
          } else {
            $xls_soci_ordering[] = array("producte" => $nomproducte, "codi" => $codiproducte, "quantitat" => $productesoci);
          }
        }
      }
      if (count($xls_soci_ordering) > 0) {
        $xls_soci[] = array("producte" => $separadors[$ordering], "codi" => "", "quantitat" => "");
        foreach ($xls_soci_ordering as $value) {
          $xls_soci[] = $value;
        }
      }
    }

    if ($centre == CENTRE_SANTS) {
      $fnum = sprintf("%03d",$soci['num']);
      $xls[$fnum." - ".$soci['nom']] = $xls_soci;
    } else {
      $xls[$soci['num']] = $xls_soci;
    }
  }

  return $xls;
}

function uc_store_socis($message = NULL, $query = NULL, $count_query = NULL, $page_length = 25) {
  $output = "";

  $centres = uc_store_get_centres();
  foreach ($centres as $centre_id => $centre_name) {
    $output .= '<a href="'.base_path().'admin/store/customers/socis/'.$centre_id.'">'.$centre_name.'</a><br/>';
  }

  return $output;
}

function uc_store_socis_centre($message = NULL, $query = NULL, $count_query = NULL, $page_length = 25) {

  $args = func_get_args();
  $arg_centre = $args[0];

  // Insertem a la taula uc_socis els usuaris donats d'alta a drupal

  $subquery = db_select('users','u')
      -> fields(  'u', array('uid') )
      -> where ( ' u.uid NOT IN (SELECT uc_socis.uid FROM uc_socis) ');
  $num = $subquery->execute()->rowCount();;
 if($num > 0 ){
// posiblemet aixo esta malament... (ull query local)
    $subquery = db_select('users','u')
    -> fields(  'u', array('uid') )
    -> where ( ' u.uid NOT IN (SELECT uc_socis.uid FROM uc_socis) ');
  $lquery = db_insert( 'uc_socis') -> fields( array( 'uid'=>  $subquery, 'centre'=> 0, 'numsoci' => 1 ));
  $queryl->execute();
  }
 // Borrem de la taula uc_socis els usuaris donats de baixa a drupal
  db_delete( 'uc_socis') -> where ("uc_socis.uid NOT IN (SELECT users.uid FROM users)")->execute();
 $header = array(
    t('Veure'),
    array('data' => t('Nom'), 'field' => 'u.name', 'sort' => 'asc'),
    array('data' => t('E-mail'), 'field' => 'u.mail'),
    array('data' => t('Centre'), 'field' => 'c_name'),
    array('data' => t('Núm. Soci'), 'field' => 's.numsoci'),
  );

  if (is_null($query)) {
   if ($arg_centre != 0) {
    //  $query = "SELECT u.uid, u.mail, u.name, s.numsoci, c.name AS namecentre, c.uid AS uidcentre "
      //    ."FROM {uc_centres} AS c, {users} AS u, {uc_socis} AS s "
        //  ."WHERE u.uid > 0 AND u.uid = s.uid AND c.uid = s.centre AND c.uid = ".$arg_centre;
        $query = db_select('uc_socis','s');
          $query->extend('TableSort')->orderByHeader($header);
      $query->join('users','u','u.uid = s.uid');
      $query->join('uc_centres','c','s.centre = c.uid');
      $query->fields('u',array('uid','mail','name'));
      $query->fields('s',array('numsoci'));
      $query->fields('c',array( 'name', 'uid'));
      $query->where ( "u.uid > 0 AND u.uid = s.uid AND c.uid = s.centre AND c.uid = :arg_centre",array(':arg_centre'=> $arg_centre));
    } else {
      //$query = "SELECT u.uid, u.mail, u.name "
        //  ."FROM {users} AS u, {uc_socis} AS s "
          //."WHERE u.uid > 0 AND u.uid = s.uid AND s.centre = 0";

  $query = db_select( 'users', 'u' );
        $query->extend('TableSort')->orderByHeader($header);
        $query->join('uc_socis','s');
  $query->fields('u',array('uid','mail','name'));
  $query->where(" u.uid > 0 AND u.uid = s.uid AND s.centre = 0 ");
    }

    $arg_centre_name = uc_store_get_centre_name($arg_centre);
    $message = t("Socis del centre: " . $arg_centre_name);
  }



  $rows = array();
  $result = $query->execute();

  foreach ($result as $customer ) {

    $icons = l(uc_store_get_icon('admin/store/customers', TRUE),
               'user/'. $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t('View user details.')))
             )
           . l(uc_store_get_icon('admin/store/orders', TRUE),
               'admin/store/customers/orders/'. $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t("View customer's order.")))
             )
           . l(uc_store_get_icon('admin/store/socis', TRUE),
               'admin/store/customers/soci/'. $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t("Veure informació del soci.")))
             );
    $name = ucfirst($customer->name);
    $mail = ucfirst($customer->mail);
    if ($arg_centre != 0) {
      $centre =  ucfirst($customer->c_name);
      $numsoci = ucfirst($customer->numsoci);
  } else {
      $centre = "";
      $numsoci = "";
  }
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($name)),
        array('data' => check_plain($mail)),
        array('data' => check_plain($centre)),
        array('data' => $numsoci)),
      'id' => 'customer-'. $customer->uid,
    );

  }




$output =    theme('table',
    array('header'=> $header,
    'rows'=>$rows,
    'attributes'=>array(),
    'caption'=>'',
    'colgroups'=>array(),
    'sticky'=>TRUE,
    'empty'=>'')) ;

  return $output;


}

// Indica si l'usuari actual és administrador (de la web, no de la botiga)
function germinal_is_admin() {
  global $user;
  if (in_array('administrador', array_values($user->roles))) {
    return true;
  } else {
    return false;
  }
}

/*
* Funció que retorna la llista de centres existents (+ "sense assignar")
*   - si $filtrar és cert només hi deixem el centre del soci i "sense assignar"
*     - excepte si és administrador de la web
*/
function uc_store_get_centres($filtrar = false) {
  $result = db_query("SELECT c.uid, c.name FROM uc_centres as c");

  $centres = array();

  if ((! $filtrar) || (germinal_is_admin())) {
    foreach ( $result as $row) {
      $centres[$row->uid] = $row->name;
    }
  } else {
    $centre_info = uc_cart_get_informacio_centre();
    $centres[$centre_info->uid] = $centre_info->name;
  }

  $centres[0] = "Sense assignar";
  return $centres;
}


function uc_store_get_rol_centre($centre) {
  $result = db_query("SELECT rid FROM role WHERE name=(SELECT rol FROM uc_centres WHERE uid= :centre )" ,array(  ':centre' => $centre ));
  foreach($result as $row){
    return $row->rid;
  }
}


function uc_store_get_informacio_soci($soci_uid) {
      $query = db_select('uc_socis' , 's');
      $query->join('users', 'u', 's.uid = u.uid AND s.uid = :soci_id', array(':soci_id'=>$soci_uid));
      $query->fields('s');
      $query->fields('u',array('name','mail'));
      $result = $query->execute();
      foreach($result as $soci){
  return $soci;
      }
    drupal_set_message(t('El soci amb uid '.$soci_uid.' no existeix.'), 'error');
}

function uc_store_assignar_soci_form($form, &$form_state , $numsoci, $centre) {

  $form['numsoci'] = array(
    '#type' => 'textfield',
    '#title' => t('Número de soci'),
    '#default_value' => $numsoci,
  '#weight' => -10,
  '#required' => TRUE,
  );

  $form['centre'] = array(
    '#type' => 'select',
    '#title' => t('Centre d\'activitat'),
    '#default_value' => $centre,
  '#required' => TRUE,
  '#weight' => 0,
    '#options' => uc_store_get_centres(true),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Assigna',
  '#weight' => 10,
  );

  return $form;
}



function uc_store_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'uc_store_assignar_soci_form')
  {
    $soci = arg(4);
    $infosoci = uc_store_get_informacio_soci($soci);
    $form['numsoci']['#value'] =  $infosoci->numsoci;
    $form['centre']['#value'] = $infosoci->centre ;
  }
}

function uc_store_assignar_soci() {
  $args = func_get_args();
  $soci = arg(4);
  $soci_info = uc_store_get_informacio_soci($soci);

  $output = "Informació del soci ".$soci_info->name." (".$soci_info->mail.")<br/>";
  $output .= drupal_render(drupal_get_form('uc_store_assignar_soci_form', $soci_info->numsoci, $soci_info->centre));
  return $output;
}

function uc_store_assignar_soci_form_validate($form, &$form_state) {
    $a = 9;
}

// We add a submit handler/function to our form.

function uc_store_assignar_soci_form_submit($form, &$form_state) {
  $soci = arg(4);
  //GERMINAL->
  // Si ja tenia un rol de germinalet l'esborro
  $query = db_delete( 'users_roles' )-> where("  uid = :soci  AND rid IN ( select rid from role where name like 'germinal_%' )", array(':soci'=>$soci));
  $query->execute();

  // Afegeixo el nou
  if ($_POST['centre']!='0')
  {
   $query = db_insert( 'users_roles')->fields( array('uid' => $soci ,'rid' => uc_store_get_rol_centre($_POST['centre']) ));
   $query->execute();
  }
    $query = db_update( 'uc_socis') -> fields(array('centre' => $_POST['centre'] , 'numsoci' => $_POST['numsoci'] ))
                         ->where( 'uid = :soci', array( ':soci'=> $soci));
    $query->execute();
    drupal_set_message(t('The form has been submitted.'));
}



function uc_store_get_informacio_centre_by_soci() {

  global $user;
  $query = db_select('uc_socis' , 'u');
  $query->condition( 'u.uid' , $user->uid , '=' );
  $query->fields('u');
  $soci = $query->fetchAll();

  $query = db_select( 'uc_centres' , 'u');
  $query->condition( 'u.uid', $soci->centre , '=' );
  $query->fields('u','*');
  $centre = $query->fetchAll();;
  return $centre;
}



function uc_store_informes() {
  $args = func_get_args();
  $setmana = arg(4);
  $centre_id = intval(arg(5));
  $centre = uc_store_get_centre_name($centre_id);

  $productes = uc_store_informe_productes($setmana, $centre_id);
  $productes_xls_filename = convert_to_excel($centre_id, $centre, $setmana, "totals", $productes);


  $productes_soci = uc_store_informe_productes_soci($setmana, $centre_id);
  if (($centre_id != CENTRE_FARRO) && ($centre_id != CENTRE_SARRIA)) {
    $productes_soci_xls_filename = convert_to_excel($centre_id, $centre, $setmana, "graella", $productes_soci, true, false); // invertim files i columnes
  } else {
    $productes_soci_xls_filename = convert_to_excel($centre_id, $centre, $setmana, "graella", $productes_soci, false, true); // no invertim files i columnes i fem les columnes petites
  }

  $socis = uc_store_informe_socis($setmana, $centre_id);
  $socis_xls_filename = convert_to_excel($centre_id, $centre, $setmana, "socis", $socis);

  // Creem el fitxer zip amb tota la informacio
  $folder = $_SERVER["DOCUMENT_ROOT"].base_path();
  $subfolder =  "sites/default/files/TMP/";

  $file = str_replace(' ', '',  $setmana."_".$centre.".zip" );
  $zip = new ZipArchive();
  $filename = $folder.$subfolder.$file;
  // que passa si el fitxer ja existeix??!!
  if ($zip->open($filename, ZIPARCHIVE::CREATE  )!==TRUE) {
      exit("cannot open <$filename>\n");
  }

  $socis_files = array();
  $res=$zip->addFile($productes_xls_filename, $setmana."_productes.xls");
  $res=$zip->addFile($productes_soci_xls_filename, $setmana."_productes_soci.xls");
  $res=$zip->addFile($socis_xls_filename, $setmana."_socis.xls");
// if it does not save, check if target folder has rwx premissions
  $res=$zip->close();

  // borrem els fitxers temporals
  unlink($productes_xls_filename);
  unlink($productes_soci_xls_filename);
  unlink($socis_xls_filename);

  global $base_url;
  $output = "<br/><br/>Fitxer creat correctament <a href=\"".$base_url."/".$subfolder.$file."\">aqui</a><br/>\n";

  return $output;

}

/* end Germinal functions*/

