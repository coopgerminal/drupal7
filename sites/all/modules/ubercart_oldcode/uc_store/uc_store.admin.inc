<?php
// $Id: uc_store.admin.inc,v 1.1.2.6 2009/01/02 20:18:40 islandusurper Exp $

/**
 * @file
 * Store administration menu items.
 *
 */

function uc_store_admin() {
  $tree = menu_navigation_links('navigation', 2);

  if (($type = variable_get('uc_store_admin_page_display', 1)) == 4) {
    $main_menu = menu_get_item('admin/store');
    $content = system_admin_menu_block((array)$main_menu);

    $output = theme('admin_block_content', $content);
  }
  else {
    $output = theme('uc_admin_dashboard', $type, $tree);
  }

  $results = module_invoke_all('store_status');

  foreach ($results as $message) {
    switch ($message['status']) {
      case 'warning':
        $icon = base_path() . drupal_get_path('module', 'uc_store') .'/images/alert.gif';
        break;
      case 'error':
        $icon = base_path() . drupal_get_path('module', 'uc_store') .'/images/error.gif';
        break;
      case 'ok':
      default:
        $icon = base_path() . drupal_get_path('module', 'uc_store') .'/images/info.gif';
    }

    $rows[] = array('data' => array(
      array('data' => '<img src="'. $icon .'" />'),
      array('data' => '<strong>'. $message['title'] .'</strong>', 'nowrap' => 'nowrap'),
      array('data' => $message['desc'], 'width' => '100%')), 'valign' => 'top');
  }

  $header = array('&nbsp;', t('Title'), t('Description'));

  $output .= '<div style="clear: both;"><h2>'. t('Status messages:') .'</h2>'
           . theme('table', $header, $rows) .'</div>';

  return $output;
}

/**
 * Display customer administration page.
 */
function uc_store_customers($message = NULL, $query = NULL, $count_query = NULL, $page_length = 25) {
  if (!module_exists('uc_order')) {
    return t('You must enable the order module to track customers.');
  }

  if (is_null($query)) {
    $query = "SELECT DISTINCT o.uid, u.mail, o.billing_first_name, "
            ."o.billing_last_name, o.billing_city, o.billing_zone, "
            ."o.billing_country FROM {uc_orders} AS o LEFT JOIN "
            ."{users} AS u ON o.uid = u.uid WHERE o.uid > 0 AND "
            ."o.order_status IN ". uc_order_status_list('general', TRUE);

    $count_query = "";
    switch ($GLOBALS['db_type']) {
      case 'mysql':
      case 'mysqli':
        $count_query = "SELECT COUNT(DISTINCT o.uid, o.billing_first_name, "
                      ."o.billing_last_name, u.mail) FROM {uc_orders} AS o "
                      ."LEFT JOIN {users} AS u ON o.uid = u.uid WHERE o.uid > 0 "
                      ."AND o.order_status IN ". uc_order_status_list('general', TRUE);
        break;
      case 'pgsql':
        $count_query = "SELECT DISTINCT o.uid, o.billing_last_name, o.billing_first_name, "
                      ."COUNT(*) "
                      ."FROM {uc_orders} AS o "
                      ."LEFT JOIN {users} AS u ON o.uid = u.uid WHERE o.uid > 0 "
                      ."AND o.order_status IN ". uc_order_status_list('general', TRUE)
                      ." GROUP BY o.uid, o.billing_last_name, o.billing_first_name ";
        break;
    }

    $message = t('The following users on your site have placed orders:');
  }

  $header = array(
    t('View'),
    array('data' => t('Name'), 'field' => 'o.billing_last_name', 'sort' => 'asc'),
    array('data' => t('E-mail'), 'field' => 'u.mail'),
    array('data' => t('City'), 'field' => 'o.billing_city'),
    array('data' => t('ID'), 'field' => 'o.uid'),
  );

  $query .= tablesort_sql($header);
  $count_query .= tablesort_sql($header);

  $address = variable_get('uc_customer_list_address', 'billing');
  if ($address == 'shipping') {
    $query = str_replace('billing', 'delivery', $query);
    $count_query = str_replace('billing', 'delivery', $count_query);
  }
  else {
    $address = 'billing';
  }

  $result = pager_query($query, $page_length, 0, $count_query);

  while ($customer = db_fetch_object($result)) {
    $icons = l(uc_store_get_icon('admin/store/customers', TRUE),
               'user/'. $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t('View user details.')))
             )
           . l(uc_store_get_icon('admin/store/orders', TRUE),
               'admin/store/customers/orders/'. $customer->uid,
               array('html' => TRUE, 'attributes' => array('title' => t("View customer's order.")))
             );
    if ($address == 'shipping') {
      $name = ucfirst($customer->delivery_last_name) .', '
            . ucfirst($customer->delivery_first_name);
      $city = ucfirst($customer->delivery_city) .', '
            . uc_get_zone_code($customer->delivery_zone);
    }
    else {
      $name = ucfirst($customer->billing_last_name) .', '
            . ucfirst($customer->billing_first_name);
      $city = ucfirst($customer->billing_city) .', '
            . uc_get_zone_code($customer->billing_zone);
    }
    if ($name == ', ') {
      $name = db_result(db_query("SELECT name FROM {users} WHERE uid = %d", $customer->uid));
      $name = t('User: !name', array('!name' => $name));
    }
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($name)),
        array('data' => check_plain($customer->mail)),
        array('data' => check_plain($city)),
        array('data' => $customer->uid)),
      'id' => 'customer-'. $customer->uid,
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_store') .'/uc_store.js');

  $output = '<p>'. $message .'</p>'
          . theme('table', $header, $rows, array('width' => '100%', 'class' => 'uc-customer-table'))
           .'<br />'. theme_pager(NULL, $page_length);

  return $output;
}

//GERMINAL ->
function uc_store_comandes($message = NULL, $query = NULL, $count_query = NULL, $page_length = 25) {

	// TODO: borrar de uc_socis els usuaris borrats de drupal

  if (is_null($query)) {
    $centre_filter = "";
	if (! germinal_is_admin()) {
		$centre_info = uc_cart_get_informacio_centre();
		$centre_filter = "AND o.centre = ".intval($centre_info->uid);
	}
    $query = "SELECT o.setmana_recollida, COUNT(o.setmana_recollida) AS quants, o.centre, c.name FROM uc_orders o, uc_centres c WHERE o.centre = c.uid ".$centre_filter." GROUP BY setmana_recollida, centre";

    $count_query = "";
    switch ($GLOBALS['db_type']) {
      case 'mysql':
      case 'mysqli':
	  	$count_query = "SELECT COUNT(DISTINCT(o.setmana_recollida)) FROM uc_orders o, uc_centres c WHERE o.centre = c.uid ".$centre_filter." GROUP BY setmana_recollida, centre";
        break;
      case 'pgsql':
/*
        $count_query = "SELECT DISTINCT o.uid, o.billing_last_name, o.billing_first_name, "
                      ."COUNT(*) "
                      ."FROM {uc_orders} AS o "
                      ."LEFT JOIN {users} AS u ON o.uid = u.uid WHERE o.uid > 0 "
                      ."AND o.order_status IN ". uc_order_status_list('general', TRUE)
                      ." GROUP BY o.uid, o.billing_last_name, o.billing_first_name ";
*/
        break;
    }

    $message = t('Setmanes de comanda:');

  }

  $header = array(
    t('Veure'),
    array('data' => t('Data'), 'field' => 'o.setmana_recollida', 'sort' => 'desc'),
    array('data' => t('Centre'), 'field' => 'c.name'),
    array('data' => t('Quantes'), 'field' => 'quants'),
  );

  $query .= tablesort_sql($header);
  $count_query .= tablesort_sql($header);

  $result = pager_query($query, $page_length, 0, $count_query);

  while ($comandes = db_fetch_object($result)) {
    $icons = l(uc_store_get_icon('admin/store/socis', TRUE),
               'admin/store/reports/comanda_setmanal/'. $comandes->setmana_recollida .'/'.$comandes->centre,
               array('html' => TRUE, 'attributes' => array('title' => t("Veure informació de les comandes de la setmana i centre.")))
             );
    $setmana = ucfirst($comandes->setmana_recollida);
    $centre = ucfirst($comandes->name);
    $quants = ucfirst($comandes->quants);
    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => check_plain($setmana)),
        array('data' => check_plain($centre)),
        array('data' => $quants)),
//      'id' => 'customer-'. $customer->uid,
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_store') .'/uc_store.js');

  $output = '<p>'. $message .'</p>'
          . theme('table', $header, $rows, array('width' => '90%', 'class' => 'uc-customer-table'))
           .'<br />'. theme_pager(NULL, $page_length);

  return $output;
}

function uc_store_get_centre_name($id) {
	if ($id == 0) {
		return "Sense assignar";
	}

	$sql = "SELECT c.name FROM {uc_centres} c WHERE uid = $id";
	$name = db_result(db_query($sql));
	return $name;
}

function uc_store_comanda_setmanal() {
	$args = func_get_args();
	$setmana = arg(4);
	$centre = uc_store_get_centre_name(intval(arg(5)));
	$missatge = "Informació de les comandes per la setmana " . $setmana . " del centre ".$centre."<br/>\n";

	// SELECT users.name, uc_socis.numsoci FROM uc_orders,uc_socis,users WHERE setmana_recollida='2009-04-27' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid
	// SELECT users.name, uc_socis.numsoci FROM uc_orders,uc_socis,users, uc_order_products WHERE setmana_recollida='2009-04-27' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.order_id = uc_order_products.order_id
/*
SELECT uc_orders.order_id
  FROM uc_orders LEFT JOIN uc_order_products ON uc_orders.order_id = uc_order_products.order_id
  WHERE uc_orders.setmana_recollida='2009-04-27'

SELECT uc_socis.numsoci, users.name, uc_orders.order_id, uc_order_products.order_id
  FROM uc_socis, users, uc_orders LEFT JOIN uc_order_products ON uc_orders.order_id = uc_order_products.order_id
  WHERE uc_orders.setmana_recollida='2009-04-27' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid

SELECT uc_socis.numsoci, users.name, uc_orders.order_id, COUNT(uc_order_products.order_id)
  FROM uc_socis, users, uc_orders LEFT JOIN uc_order_products ON uc_orders.order_id = uc_order_products.order_id
  WHERE uc_orders.setmana_recollida='2009-04-27' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid GROUP BY uc_orders.order_id

*/
	// aquesta query retorna els socis que han fet comanda una setmana en un centre i la quantitat de productes demanats,
	// encara que no hagin demanat cap producte, això permet detectar problemes
	$query = 	"SELECT uc_socis.numsoci, users.name, uc_orders.order_id, COUNT(uc_order_products.order_id) AS numproductes, uc_orders.order_status" .
  			"	FROM uc_socis, users, uc_orders " .
  					" LEFT JOIN uc_order_products ON uc_orders.order_id = uc_order_products.order_id " .
  			"	WHERE uc_orders.setmana_recollida='".$setmana."' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.centre = ".intval(arg(5))." GROUP BY uc_orders.order_id ";

/*
    $result = db_query(db_rewrite_sql($sql));
    while ($data = db_fetch_object($result)) {
		$output .= $data->numproductes . "<br/>\n";
    }
*/

  $header = array(
    t('Veure'),
    array('data' => t('Nom'), 'field' => 'users.name', 'sort' => 'asc'),
    array('data' => t('Núm. Soci'), 'field' => 'uc_socis.numsoci'),
    array('data' => t('Núm Productes'), 'field' => 'numproductes'),
    array('data' => t('Estat'), 'field' => 'uc_orders.order_status')
  );

  $query .= tablesort_sql($header);
  $result = db_query(db_rewrite_sql($query));

  global $base_url;
  while ($comandasoci = db_fetch_object($result)) {
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

  drupal_add_js(drupal_get_path('module', 'uc_store') .'/uc_store.js');

  $output = '<p>'. $missatge .'</p>'
          . theme('table', $header, $rows, array('width' => '90%', 'class' => 'uc-customer-table'))
           .'<br />'. theme_pager(NULL, $page_length);

	$output .= "<a href=\"?q=admin/store/reports/informes/".$setmana."/".intval(arg(5))."\">Generar llistats</a>";


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

	$export_filename = tempnam("tmp/","germinal_");
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
	$subfolder = "files/comandes/";
	$file = $setmana."_".$centre.".zip";
	$zip = new ZipArchive();
	$filename = $folder.$subfolder.$file;

	// que passa si el fitxer ja existeix??!!
	if ($zip->open($filename, ZIPARCHIVE::CREATE)!==TRUE) {
	    exit("cannot open <$filename>\n");
	}

	$socis_files = array();
	$res=$zip->addFile($productes_xls_filename, $setmana."_productes.xls");
	$res=$zip->addFile($productes_soci_xls_filename, $setmana."_productes_soci.xls");
	$res=$zip->addFile($socis_xls_filename, $setmana."_socis.xls");
	$res=$zip->close();

	// borrem els fitxers temporals
	unlink($productes_xls_filename);
	unlink($productes_soci_xls_filename);
	unlink($socis_xls_filename);

	global $base_url;
	$output = "<br/><br/>Fitxer creat correctament <a href=\"".$base_url."/".$subfolder.$file."?foo=".time()."\">aqui</a><br/>\n";

	return $output;
}

function uc_store_format_numeros($valor) {
	return number_format($valor, 2, ',', '');
}

function uc_store_get_nom_separadors($centre) {
	// Seleccionem el nom dels separadors
	$sql = 	"SELECT node.title AS nom, uc_products.ordering " .
				"FROM node, uc_products " .
				"WHERE node.nid = uc_products.nid AND uc_products.separador=1 AND uc_products.centre = ".$centre;

	$separadors = array();

    $result = db_query(db_rewrite_sql($sql));
    while ($data = db_fetch_object($result)) {
    	$separadors[$data->ordering+1] = $data->nom;
	}

	return $separadors;
}

// Aquesta funció agafa la suma de cada producte dels demanats per un centre i setmana
function uc_store_informe_productes($setmana, $centre) {
	// Seleccionem la suma de les quantitats dels productes per una setmana de recollida i un centre concret
	$sql = 	"SELECT uc_order_products.title AS producte, SUM(uc_order_products.qty) AS quantitat, uc_products.ordering, uc_products.codi " .
				"FROM uc_orders, uc_order_products, uc_products " .
				"WHERE uc_order_products.nid = uc_products.nid AND uc_orders.setmana_recollida='".$setmana."' AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_orders.order_status LIKE 'pending'".
				"GROUP BY uc_order_products.nid ".
				"ORDER BY uc_products.ordering ASC, uc_order_products.title ASC";

	// La capçalera de la taula només conté el producte i la quantitat
	$xsl = array();

	// Per cada producte posem el nom i la quantitat
///    $result = db_query(db_rewrite_sql($sql));
    $result = db_query($sql);
    while ($data = db_fetch_object($result)) {
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

define(CENTRE_SANTS, 1);
define(CENTRE_FARRO, 2);
define(CENTRE_SARRIA, 4);

// Aquesta funció agafa la graella de productes/soci per a preparar la comanda
function uc_store_informe_productes_soci($setmana, $centre) {
	// Seleccionem per cada soci d'un centre els productes demanats per una setmana de recollida
	// el resultat és redundant, però així fem només una consulta
	$sql = 	"SELECT uc_socis.numsoci, users.name, uc_orders.order_id, uc_order_products.title, uc_order_products.qty, uc_products.codi, uc_products.ordering " .
			"FROM uc_socis, users, uc_orders, uc_order_products, uc_products " .
			"WHERE uc_orders.setmana_recollida='$setmana' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_products.nid = uc_order_products.nid AND uc_orders.order_status LIKE 'pending' ".
			"ORDER BY uc_products.ordering ASC, uc_order_products.title ASC";

	// El primer que fem es recorrer el resultat i crear un llistat de productes i per cada soci els productes que ha demanat
	$socis = array();
	$productes = array();
///    $result = db_query(db_rewrite_sql($sql));
    $result = db_query($sql);
    while ($data = db_fetch_object($result)) {
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

    	$soci = $socis[$numsoci];
    	if ($soci == null) {
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
	$sql = 	"SELECT uc_socis.numsoci, users.name, uc_orders.order_id, uc_order_products.title, uc_order_products.qty, uc_products.codi, uc_products.ordering " .
			"FROM uc_socis, users, uc_orders, uc_order_products, uc_products " .
			"WHERE uc_orders.setmana_recollida='$setmana' AND uc_orders.uid = uc_socis.uid AND uc_orders.uid = users.uid AND uc_orders.order_id = uc_order_products.order_id AND uc_orders.centre = $centre AND uc_products.nid = uc_order_products.nid AND uc_orders.order_status LIKE 'pending' ".
			"ORDER BY uc_socis.numsoci ASC, uc_products.ordering ASC, uc_order_products.title ASC";

	// El primer que fem es recorrer el resultat i crear un llistat de productes i per cada soci els productes que ha demanat
	$socis = array();
	$productes = array();
///    $result = db_query(db_rewrite_sql($sql));
    $result = db_query($sql);
    while ($data = db_fetch_object($result)) {
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

    	$soci = $socis[$numsoci."-".$nomsoci];
    	if ($soci == null) {
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
	$update_query = "INSERT INTO uc_socis (uid) (SELECT users.uid FROM users WHERE users.uid NOT IN (SELECT uc_socis.uid FROM uc_socis))";
	db_query($update_query);

	// Borrem de la taula uc_socis els usuaris donats de baixa a drupal
	$update_query = "DELETE FROM uc_socis WHERE uc_socis.uid NOT IN (SELECT users.uid FROM users)";
	db_query($update_query);


	if (is_null($query)) {
		if ($arg_centre != 0) {
			$query = "SELECT u.uid, u.mail, u.name, s.numsoci, c.name AS namecentre, c.uid AS uidcentre "
					."FROM {uc_centres} AS c, {users} AS u, {uc_socis} AS s "
					."WHERE u.uid > 0 AND u.uid = s.uid AND c.uid = s.centre AND c.uid = ".$arg_centre;
		} else {
			$query = "SELECT u.uid, u.mail, u.name "
					."FROM {users} AS u, {uc_socis} AS s "
					."WHERE u.uid > 0 AND u.uid = s.uid AND s.centre = 0";
		}

		$arg_centre_name = uc_store_get_centre_name($arg_centre);
		$message = t("Socis del centre: " . $arg_centre_name);
	}


  $header = array(
    t('Veure'),
    array('data' => t('Nom'), 'field' => 'u.name', 'sort' => 'asc'),
    array('data' => t('E-mail'), 'field' => 'u.mail'),
    array('data' => t('Centre'), 'field' => 'c.name'),
    array('data' => t('Núm. Soci'), 'field' => 's.numsoci'),
  );

  $query .= tablesort_sql($header);
  $count_query .= tablesort_sql($header);

  $result = pager_query($query, $page_length, 0, NULL);

  while ($customer = db_fetch_object($result)) {
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
	    $centre = ucfirst($customer->namecentre);
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

  drupal_add_js(drupal_get_path('module', 'uc_store') .'/uc_store.js');

  $output = '<p>'. $message .'</p>'
          . theme('table', $header, $rows, array('width' => '90%', 'class' => 'uc-customer-table'))
           .'<br />'. theme_pager(NULL, $page_length);

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
* 	- si $filtrar és cert només hi deixem el centre del soci i "sense assignar"
* 		- excepte si és administrador de la web
*/
function uc_store_get_centres($filtrar = false) {
	$result = db_query("SELECT c.uid, c.name FROM {uc_centres} c");

	$centres = array();
	if ((! $filtrar) || (germinal_is_admin())) {
		while ($row = db_fetch_object($result)) {
		  $centres[$row->uid] = $row->name;
		}
	} else {
		$centre_info = uc_cart_get_informacio_centre();
		$centres[$centre_info->uid] = $centre_info->name;
	}

	$centres[0] = "Sense assignar";
	return $centres;
}

//GERMINAL->
function uc_store_get_rol_centre($centre) {
	$result = db_query("SELECT rid FROM role WHERE name=(SELECT rol FROM uc_centres WHERE uid=".$centre.")");
	$row = db_fetch_object($result);
	return $row->rid;
}
//GERMINAL<-

function uc_store_assignar_soci_form($state) {
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
    '#default_value' => 1,
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

function uc_store_get_informacio_soci($soci_id) {
	$result = db_query("SELECT s.*, u.name, u.mail FROM {uc_socis} s, {users} u WHERE s.uid = u.uid AND s.uid = " . $soci_id);
	$soci = db_fetch_object($result);
	return $soci;
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
	$output .= drupal_get_form('uc_store_assignar_soci_form');
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
	$update_query = "DELETE FROM users_roles WHERE uid = ".$soci." AND rid IN(SELECT rid FROM role WHERE name LIKE 'germinal_%')";
	db_query($update_query);

	// Afegeixo el nou
	if ($_POST['centre']!='0')
	{
		$update_query = "INSERT INTO users_roles(uid, rid) VALUES (".$soci.", ".uc_store_get_rol_centre($_POST['centre']).")";
		db_query($update_query);
	}
	//GERMINAL<-

	$update_query = "UPDATE {uc_socis} s SET centre = ".$_POST['centre']." , numsoci = ".$_POST['numsoci'] . " WHERE s.uid = ".$soci;
	db_query($update_query);

    drupal_set_message(t('The form has been submitted.'));
}
//GERMINAL <-


/**
 * Display the customer search page.
 */
function uc_store_customer_search() {
  $output = drupal_get_form('uc_store_customer_search_form');

  if (arg(4) == 'results') {
    $first_name = strtolower(str_replace('*', '%', check_plain(arg(5))));
    $last_name = strtolower(str_replace('*', '%', check_plain(arg(6))));
    $email = strtolower(str_replace('*', '%', check_plain(arg(7))));

    if ($first_name !== '0' && $first_name !== '%') {
      $where .= " AND LOWER(o.billing_first_name) LIKE '". $first_name ."'";
    }
    if ($last_name !== '0' && $last_name !== '%') {
      $where .= " AND LOWER(o.billing_last_name) LIKE '". $last_name ."'";
    }
    if ($email !== '0' && $email !== '%') {
      $where .= " AND LOWER(o.primary_email) LIKE '". $email ."'";
    }

    $query = "SELECT DISTINCT o.uid, u.mail, o.billing_first_name,"
            ."o.billing_last_name, o.billing_city, o.billing_zone, "
            ."o.billing_country FROM {uc_orders} AS o LEFT JOIN "
            ."{users} AS u ON o.uid = u.uid WHERE o.uid > 0 AND "
            ."o.order_status IN ". uc_order_status_list('general', TRUE)
           . $where;// ." ORDER BY o.billing_last_name ASC";


    $count_query = '';
    switch ($GLOBALS['db_type']) {
      case 'mysql':
      case 'mysqli':
        $count_query = "SELECT COUNT(DISTINCT o.uid, o.billing_first_name, "
                       ."o.billing_last_name, u.mail) FROM {uc_orders} AS o "
                       ."LEFT JOIN {users} AS u ON o.uid = u.uid WHERE o.uid > 0 AND "
                       ."o.order_status IN ". uc_order_status_list('general', TRUE)
                       . $where;// ." ORDER BY o.billing_last_name ASC";
        break;
      case 'pgsql':
        $count_query = "SELECT DISTINCT o.uid, o.billing_first_name, "
                      ."o.billing_last_name, u.mail, COUNT(*) "
                      ."FROM {uc_orders} AS o "
                      ."LEFT JOIN {users} AS u ON o.uid = u.uid WHERE o.uid > 0 AND "
                      ."o.order_status IN ". uc_order_status_list('general', TRUE)
                      . $where
                      ."GROUP BY o.uid, o.billing_first_name, o.billing_last_name, u.mail ";
                      //."ORDER BY o.billing_last_name ASC";
        break;
    }

    $message = t('Search returned the following results:');

    $output .= uc_store_customers($message, $query, $count_query, 100);
  }

  return $output;
}

function uc_store_customer_search_form() {
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search options'),
    '#collapsible' => TRUE,
    '#collapsed' => arg(4) == 'results' ? TRUE : FALSE,
  );

  $form['search']['table1'] = array('#value' => '<table><tbody style="border: 0px;"><tr><td colspan="4">');

  $form['search']['desc'] = array(
    '#value' => '<div>'. t("Search for customers based on any of the following fields. Use * as a wildcard to match any character.<br/>For example, searching by last name for 's*' will return all customers whose last name starts with an s.<br/>(<em>Leave a field empty to ignore it in the search.</em>)") .'</div>',
  );

  $form['search']['table2'] = array('#value' => '</td></tr><tr><td>');

  $form['search']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => arg(5) != '0' ? arg(5) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['table3'] = array('#value' => '</td><td>');

  $form['search']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => arg(6) != '0' ? arg(6) : '',
    '#size' => 24,
    '#maxlength' => 32,
  );

  $form['search']['table4'] = array('#value' => '</td><td>');

  $form['search']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#default_value' => arg(7) != '0' ? arg(7) : '',
    '#size' => 24,
    '#maxlength' => 96,
  );

  $form['search']['table5'] = array('#value' => '</td><td>');

  $form['search']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  $form['search']['table6'] = array('#value' => '</td></tr></tbody></table>');

  return $form;
}

function uc_store_customer_search_form_submit($form, &$form_state) {
  if (strlen(trim($form_state['values']['first_name'])) == 0) {
    $first_name = '0';
  }
  else {
    $first_name = strtolower(trim($form_state['values']['first_name']));
  }

  if (strlen(trim($form_state['values']['last_name'])) == 0) {
    $last_name = '0';
  }
  else {
    $last_name = strtolower(trim($form_state['values']['last_name']));
  }

  if (strlen(trim($form_state['values']['email'])) == 0) {
    $email = '0';
  }
  else {
    $email = strtolower(trim($form_state['values']['email']));
  }

  drupal_goto('admin/store/customers/search/results/'. $first_name .'/'
            . $last_name .'/'. $email);
}

/**
 * Display main reports page.
 */
function uc_store_reports() {
  $menu = menu_get_item('admin/store/reports');
  $content = system_admin_menu_block($menu);

  $message = (module_exists('uc_reports')) ? t('Various reports generated by Ubercart modules can be found here. Click the links below to view the reports.') : t('Various reports generated by Ubercart modules can be found here. Click the links below to view the reports. To view core Ubercart statistics enable the <strong>Reports</strong> module on the <a href="!url">module administration page</a>', array('!url' => url('admin/build/modules')));
  $output = $message . theme('admin_block_content', $content);

  return $output;
}

/**
 * Display store configuration page.
 */
function uc_store_configuration_page() {
  $menu = menu_get_item('admin/store/settings');
  $content = system_admin_menu_block($menu);

  $output = theme('admin_block_content', $content);

  return $output;
}

/**
 * Display store help page.
 */
function uc_store_ubercart_help() {
  $output = '<p>'. t('Use the following links to find documentation and support:') .'</p>';

  $items[] = l(t("Ubercart User's Guide"), 'http://www.ubercart.org/docs/user');
  $items[] = l(t('Support Forums'), 'http://www.ubercart.org/forum');
  $items[] = l(t('Drupal Handbook'), 'http://drupal.org/node/258');
  $output .= theme_item_list($items);

  return $output;
}

/**
 * Display the tokens help page.
 */
function uc_store_ubercart_help_tokens() {
  $output = '<p>'. t('Tokens are bracketed phrases you can use in certain text fields and boxes as placeholders for some other text. Tokens represent things like store variables, links to certain pages, order information, etc.  Tokens are used by including the token listed below in a text field that uses them.  The description for the textfields will alert you to which groups of tokens listed below may be used.') .'</p>';

  $output .= theme('token_help', 'ubercart');

  return $output;
}

// Displays an overview of the country settings.
function uc_country_settings_overview() {
  // Theme all the pages beneath this path into summary overviews.
  return theme('summary_overview', summarize_child_form_pages('admin/store/settings/countries/edit'));

}

function uc_country_import_form() {
  $result = db_query("SELECT * FROM {uc_countries} ORDER BY country_name ASC");
  while ($country = db_fetch_object($result)) {
    $countries[] = $country;
  }
  $files = _country_import_list();

  $header = array(t('Country'), t('Code'), t('Version'), t('Operations'));
  if (is_array($countries)) {
    foreach ($countries as $country) {
      $row = array(
        $country->country_name,
        $country->country_iso_code_3,
        array('data' => abs($country->version), 'align' => 'center')
      );

      $ops = array();
      if ($country->version < 0) {
        $ops[] = l(t('enable'), 'admin/store/settings/countries/'. $country->country_id .'/enable');
      }
      else {
        $ops[] = l(t('disable'), 'admin/store/settings/countries/'. $country->country_id .'/disable');
      }
      if ($country->version < $files[$country->country_id]['version'] && $country->version > 0) {
        $ops[] = l(t('update'), 'admin/store/settings/countries/'. $country->country_id .'/update/'. $files[$country->country_id]['version']);
      }
      $ops[] = l(t('remove'), 'admin/store/settings/countries/'. $country->country_id .'/remove');
      $row[] = implode(' ', $ops);

      $rows[] = $row;
      unset($files[$country->country_id]);
    }
  }

  foreach ($files as $file) {
    $import_list[$file['file']] = $file['file'];
  }
  if (is_array($import_list)) {
    ksort($import_list);
  }

  if (is_array($import_list)) {
    $options = $import_list;
  }
  else {
    $options = array(t('-None available-'));
  }

  $form['country_import'] = array(
    '#title' => 'Country importer',
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#summary callback' => '_uc_country_summarize',
  );

  $form['country_import']['text'] = array(
    '#value' => t('To import new country data, select it in the list and click the import button. If you are using a custom or contributed import file, it must be placed in the Ubercart folder uc_store/countries.'),
  );
  $form['country_import']['import_file'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#options' => $options,
    '#disabled' => !is_array($import_list),
    '#multiple' => is_array($import_list),
    '#size' => min(10, count($options)),
  );
  $form['country_import']['import_button'] = array(
    '#type' => 'submit',
    '#value' => t('Import'),
    '#disabled' => !is_array($import_list),
  );
  $form['country_table'] = array(
    '#value' => theme('table', $header, $rows),
  );

  return $form;
}

function _uc_country_summarize() {

  $items = array();

  $result = db_query("SELECT * FROM {uc_countries} ORDER BY country_name ASC");
  while ($country = db_fetch_object($result)) {

    $items[] = $country->version > 0
    ? t(
      '!country version !version is enabled.',
      array('!country' => $country->country_name, '!version' => abs($country->version))
    )
    : t(
      '!country version !version is disabled.',
      array('!country' => $country->country_name, '!version' => abs($country->version))
    );
  }

  return $items;
}

function uc_country_import_form_submit($form, &$form_state) {
  $files = $form_state['values']['import_file'];

  foreach ($files as $file) {
    $pieces = explode('_', substr($file, 0, strlen($file) - 4));
    $country_id = $pieces[count($pieces) - 2];
    $version = $pieces[count($pieces) - 1];
    $country = substr($file, 0, strlen($file) - strlen($country_id) - strlen($version) - 6);

    require_once(drupal_get_path('module', 'uc_store') .'/countries/'. $file);
    $func = $country .'_install';
    if (function_exists($func)) {
      $func();
      drupal_set_message(t('Country file @file imported.', array('@file' => $file)));
    }
    else {
      drupal_set_message(t('Country file @file had no install function.', array('@file' => $file)), 'error');
    }
  }
}

function uc_country_formats_form() {
  $form['instructions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Address variables instructions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#summary' => 'Tweak the address formatting for a specific country.',
    '#summary arguments' => array(FALSE),
  );
  $header = array(t('Variable'), t('Description'));
  $rows = array(
    array('!first_name', t("Customer's first name")),
    array('!last_name', t("Customer's last name")),
    array('!company', t('Company name')),
    array('!street1', t('First street address field')),
    array('!street2', t('Second street address field')),
    array('!city', t('City name')),
    array('!zone_name', t('Full name of the zone')),
    array('!zone_code', t('Abbreviation of the zone')),
    array('!postal_code', t('Postal code')),
    array('!country_name', t('Name of the country')),
    array('!country_code2', t('2 digit country abbreviation')),
    array('!country_code3', t('3 digit country abbreviation')),
  );
  $form['instructions']['text'] = array(
    '#value' => '<div>'. t('The following variables should be used in configuring addresses for the countries you ship to:') .'<p>'
              . theme('table', $header, $rows) .'</p><p>'. t('*Adding _if to any country variable will make it only display for addresses<br />whose country is different than the default store country.') .'</div>',
  );

  $result = db_query("SELECT * FROM {uc_countries} ORDER BY country_name ASC");
  while ($country = db_fetch_object($result)) {
    $countries[] = $country;
  }

  if (is_array($countries)) {
    $form['countries'] = array(
      '#tree' => TRUE,
    );
    foreach ($countries as $country) {
      $form['countries'][$country->country_id] = array(
        '#type' => 'fieldset',
        '#title' => $country->country_name,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#summary callback' => 'summarize_null',
      );
      $form['countries'][$country->country_id]['address_format'] = array(
        '#type' => 'textarea',
        '#title' => t('Address format'),
        '#default_value' => variable_get('uc_address_format_'. $country->country_id, ''),
        '#description' => t('Uses the variables mentioned in the instructions to format an address for this country.'),
        '#rows' => 6,
      );
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit changes'),
  );

  return $form;
}

function uc_country_formats_form_submit($form, &$form_state) {
  foreach ($form_state['values']['countries'] as $country_id => $data) {
    variable_set('uc_address_format_'. $country_id, $data['address_format']);
  }
  drupal_set_message(t('Country settings saved.'));
}

// Displays an overview of the store settings.
function uc_store_store_settings_overview() {
  // Theme all the pages beneath this path into summary overviews.
  return theme('summary_overview', summarize_child_form_pages('admin/store/settings/store/edit'));
}

/**
 * Summarizes the store's address settings.
 *
 * @param $form
 *   The form passed from the summarizer
 * @return
 *   An array of summary information
 */
function _uc_store_address_summarize($form) {
  $address = trim(uc_store_address());

  return array(
    'data' => t('Store address:<br />!address', array('!address' => empty($address) ? t('Not set yet.') : $address)),
  );
}

// Displays the form for store settings.
function uc_store_store_settings_form() {
  $form['uc_store_name'] = uc_textfield(t('Store name'), variable_get('uc_store_name', NULL), FALSE, NULL, 64);
  $form['uc_store_owner'] = uc_textfield(t('Store owner'), variable_get('uc_store_owner', NULL), FALSE, NULL, 64);

  $form['uc_store_email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail address'),
    '#description' => NULL,
    '#size' => 32,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#default_value' => variable_get('uc_store_email', NULL),
    '#summary' => variable_get('uc_store_email', NULL) ? t('Store e-mail address is %email.', array('%email' => variable_get('uc_store_email', ''))) : t('Store e-mail address is not set.'),
  );

  $form['uc_store_email_include_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include the store name in the from line of store e-mails.'),
    '#description' => t('May not be available on all server configurations. Turn off if this causes problems.'),

    // don't show this element if there isn't an e-mail address set
    '#summary callback' => variable_get('uc_store_email', NULL) ? NULL : 'summarize_null',
    '#default_value' => variable_get('uc_store_email_include_name', TRUE),
    '#summary callback' => variable_get('uc_store_email', NULL) ? NULL : 'summarize_null',
  );

  $form['uc_store_phone'] = uc_textfield(t('Phone number'), variable_get('uc_store_phone', NULL), FALSE);
  $form['uc_store_fax'] = uc_textfield(t('Fax number'), variable_get('uc_store_fax', NULL), FALSE);

  $form['store_address'] = array('#summary callback' => '_uc_store_address_summarize');
  $form['store_address']['uc_store_street1'] = uc_textfield(uc_get_field_name('street1'), variable_get('uc_store_street1', NULL), FALSE, NULL, 128);
  $form['store_address']['uc_store_street2'] = uc_textfield(uc_get_field_name('street2'), variable_get('uc_store_street2', NULL), FALSE, NULL, 128);
  $form['store_address']['uc_store_city'] = uc_textfield(uc_get_field_name('city'), variable_get('uc_store_city', NULL), FALSE);
  $form['store_address']['uc_store_country'] = uc_country_select(uc_get_field_name('country'), uc_store_default_country());

  if (isset($_POST['uc_store_country'])) {
    $country_id = intval($_POST['uc_store_country']);
  }
  else {
    $country_id = uc_store_default_country();
  }
  $form['store_address']['uc_store_zone'] = uc_zone_select(uc_get_field_name('zone'), variable_get('uc_store_zone', NULL), NULL, $country_id);
  $form['store_address']['uc_store_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), variable_get('uc_store_postal_code', NULL), FALSE, NULL, 10);

  $form['uc_notify_store_help_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Store help page'),
    '#description' => t('The Drupal page for the store help link.'),
    '#default_value' => variable_get('uc_notify_store_help_page', ''),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
    '#summary' => variable_get('uc_notify_store_help_page', '') ? t('Help page is <a href="!url">!url</a>.', array('!url' => url(variable_get('uc_notify_store_help_page', ''), array('absolute' => TRUE)))) : t('Help page is not set.'),
  );

  return system_settings_form($form);
}

function uc_store_display_settings_form() {

  $display_options = array(
    1 => t('Dashboard with collapsed submenu links'),
    2 => t('Dashboard with expanded submenu links'),
    3 => t('Dashboard with no submenu links'),
    4 => t('Normal Drupal submenu listing'),
  );

  $form['uc_store_admin_page_display'] = array(
    '#type' => 'radios',
    '#title' => t('Display type for the main store admininstration page'),
    '#description' => t('Some options are better suited for different themes, so feel free to try them all out!'),
    '#options' => $display_options,
    '#summary' => t('Store admin page is displaying: <br />@display', array('@display' => $display_options[variable_get('uc_store_admin_page_display', 1)])),
    '#default_value' => variable_get('uc_store_admin_page_display', 1),
  );

  $address_options = array(
    'billing' => t('Billing address'),
    'shipping' => t('Shipping address'),
  );

  $desc = t('Select the address to be used on customer lists and summaries.');
  if (module_exists('uc_notify')) {
    $desc .= ' '. t('Also applies to notification e-mails.');
  }
  $form['uc_customer_list_address'] = array(
    '#type' => 'radios',
    '#title' => t('Primary customer address'),
    '#description' => $desc,
    '#options' => $address_options,
    '#summary' => t('Customer\'s %billing is being used in lists.', array('%billing' => $address_options[variable_get('uc_customer_list_address', 'billing')])),
    '#default_value' => variable_get('uc_customer_list_address', 'billing'),
  );

  $options = array_merge(array(t('Randomly select a message from the list below.')), _store_footer_options());

  // format the message nicely for the user
  switch (variable_get('uc_footer_message', 0)) {
    case 'none':
      $user_footer = 'no message.';
    break;

    case 0:
      $user_footer = 'a random message.';
    break;

    default:
      $user_footer = '"'. $options[variable_get('uc_footer_message', 0)] .'"';
    break;
  }

  $form['uc_footer_message'] = array(
    '#type' => 'radios',
    '#title' => t('Footer message for store pages'),
    '#options' => $options,
    '#summary' => t('Footer using !footer', array('!footer' => $user_footer)),
    '#default_value' => variable_get('uc_footer_message', 0),
  );

  return system_settings_form($form);
}

function uc_store_format_settings_form() {
  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency format'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
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
    '#summary' => t('Currency format: @format', array('@format' => uc_currency_format(1000.1234))),
    '#disabled' => TRUE,
    '#size' => 10,
  );
  $form['currency']['uc_currency_sign'] = uc_textfield(t('Currency Sign'), variable_get('uc_currency_sign', '$'), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_sign']['#summary callback'] = 'summarize_null';

  $form['currency']['uc_sign_after_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display currency sign after amount.'),
    '#summary callback' => 'summarize_null',
    '#default_value' => variable_get('uc_sign_after_amount', FALSE),
  );

  $form['currency']['uc_currency_thou'] = uc_textfield(t('Thousands Marker'), variable_get('uc_currency_thou', ','), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_thou']['#summary callback'] = 'summarize_null';

  $form['currency']['uc_currency_dec'] = uc_textfield(t('Decimal Marker'), variable_get('uc_currency_dec', '.'), FALSE, NULL, 10, 10);
  $form['currency']['uc_currency_dec']['#summary callback'] = 'summarize_null';

  $form['currency']['uc_currency_prec'] = array(
    '#type' => 'select',
    '#title' => t('Number of decimal places'),
    '#options' => drupal_map_assoc(array(0, 1, 2)),
    '#summary callback' => 'summarize_null',
    '#default_value' => variable_get('uc_currency_prec', 2),
  );

  $form['weight'] = array(
    '#type' => 'fieldset',
    '#title' => t('Weight format'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['weight']['instructions'] = array(
    '#value' => '<div>'. t('Supply a format string for each unit. !value represents the weight value.') .'</div>',
    '#summary callback' => 'summarize_null',
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
    '#summary' => t('Weight format: @weight', array('@weight' => uc_weight_format(36))),
    '#default_value' => variable_get('uc_weight_unit', 'lb'),

    '#options' => $units,
  );
  foreach ($units as $unit => $name) {
    $form['weight']['uc_weight_format_'. $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#summary callback' => 'summarize_null',
      '#default_value' => variable_get('uc_weight_format_'. $unit, '!value '. $unit),
    );
  }

  $form['length'] = array(
    '#type' => 'fieldset',
    '#title' => t('Length format'),
    '#summary callback' => 'summarize_null',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['length']['instructions'] = array(
    '#value' => '<div>'. t('Supply a format string for each unit. !value represents the weight value.') .'</div>',
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
    $form['length']['uc_length_format_'. $unit] = array(
      '#type' => 'textfield',
      '#title' => t('@unit format string', array('@unit' => $name)),
      '#default_value' => variable_get('uc_store_length_format_'. $unit, '!value '. $unit),
    );
  }

  $form['date'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date format'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['date']['instructions'] = array(
    '#value' => '<div>'. t('Supply a format string using !link syntax.', array('!link' => l(t('PHP date'), 'http://www.php.net/date'))) .'</div>',
    '#summary callback' => 'summarize_form',
  );
  $form['date']['uc_date_format_default'] = array(
    '#type' => 'textfield',
    '#title' => t('Default format string'),
    '#default_value' => variable_get('uc_date_format_default', 'm/d/Y'),
    '#summary' => t('Date format: @date', array('@date' => uc_date_format(8, 18, 2007))),
  );

  return system_settings_form($form);
}

function uc_store_report_settings() {
  $form['uc_store_report'] = array(
    '#type' => 'checkbox',
    '#title' => t('Report version information to Ubercart.org as shown below.'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Version information is being reported to Ubercart.org.'),
      t('Version information is not being reported to Ubercart.org.'),
    ),
    '#default_value' => variable_get('uc_store_report', TRUE),
  );

  $form['data'] = array(
    '#type' => 'fieldset',
    '#summary callback' => 'summarize_null',
    '#title' => t('Report preview'),
  );
  $form['data']['preview'] = array(
    '#value' => '<pre>'. print_r(uc_store_report_data(), TRUE) .'</pre>',
  );

  return system_settings_form($form);
}

/**
 * Form to enter initials for an administrative user.
 */
function uc_store_initials() {
  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('User name'),
    '#description' => t('Enter the name of the user whose initials you want to adjust.'),
    '#required' => TRUE,
    '#size' => 32,
    '#autocomplete_path' => 'user/autocomplete',
  );
  $form['initials'] = array(
    '#type' => 'textfield',
    '#title' => t('Initials'),
    '#description' => t('Enter initials or leave blank to erase current initials.'),
    '#size' => 6,
    '#maxlength' => 32,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function uc_store_initials_submit($form, &$form_state) {
  $result = db_query("SELECT uid FROM {users} WHERE name = '%s'", $form_state['values']['username']);
  if ($user = db_fetch_object($result)) {
    if ($form_state['values']['initials'] == '') {
      variable_del('user_initials_'. $user->uid);
      drupal_set_message(t('Initials for !username deleted.', array('!username' => $form_state['values']['username'])));
    }
    else {
      variable_set('user_initials_'. $user->uid, $form_state['values']['initials']);
      drupal_set_message(t('Initials for !username set to !initials.',
                           array('!username' => $form_state['values']['username'],
                                 '!initials' => $form_state['values']['initials'])));
    }
  }
}

/**
 * List of extensible TAPIr tables used by Ubercart.
 */
function uc_store_tables() {
  $output = '<p>'. t('The following tables are used to display various parts of your store to your administrators and customers. Click on a table id to configure the display of that table.');
  $output .= '<p>'. tapir_table_list('admin/store/settings/tables') .'</p>';

  return $output;
}

function uc_store_customer_orders($uid) {
  $result = pager_query("SELECT * FROM {uc_orders} WHERE uid = %d AND "
                       ."order_status IN ". uc_order_status_list('general', TRUE)
                       ." ORDER BY created DESC", 50, 0,
                        NULL, $uid);

  $header = array(t('View'), t('Order ID'), t('Date'), t('Billing name'),
                  t('Shipping name'), t('Items'), t('Total'));

  $totals = array('orders' => 0, 'items' => 0, 'total' => 0);
  while ($order = db_fetch_object($result)) {
    $icons = l(uc_store_get_icon('file:order_view'),
               'admin/store/orders/'. $order->order_id,
               array('html' => TRUE, 'attributes' => array('title' => t("View order !order_id.", array('!order_id' => $order->order_id))))
             )
           . l(uc_store_get_icon('file:order_edit'),
               'admin/store/orders/'. $order->order_id .'/edit',
               array('html' => TRUE, 'attributes' =>
                 array(
                   'title' => t("Edit order !order_id.",
                     array('!order_id' => $order->order_id)
                   )
                 )
               )
             );
    $bname = ucfirst($order->billing_first_name) .' '
           . ucfirst($order->billing_last_name);
    $sname = ucfirst($order->delivery_first_name) .' '
           . ucfirst($order->delivery_last_name);
    $result2 = db_query("SELECT COUNT(*) FROM {uc_order_products} WHERE "
                       ."order_id = %d", $order->order_id);
    $item_count = db_fetch_array($result2);

    $totals['orders'] += 1;
    $totals['items'] += $item_count['COUNT(*)'];
    $totals['total'] += $order->order_total;

    $rows[] = array(
      'data' => array(
        array('data' => $icons),
        array('data' => $order->order_id),
        array('data' => format_date($order->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y'))),
        array('data' => check_plain($bname)),
        array('data' => check_plain($sname)),
        array('data' => $item_count['COUNT(*)']),
        array('data' => uc_currency_format($order->order_total), 'nowrap' => 'nowrap')),
      'id' => 'order-'. $order->order_id,
    );
  }
  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('No orders found.'), 'colspan' => 7)
    );
  }

  drupal_add_js(drupal_get_path('module', 'uc_store') .'/uc_store.js');

  $output = '<p>'. l(t('Create an order for this customer.'),
                     'admin/store/orders/create/'. $uid) .'</p>';
  $output .= '<p>'. t('!totals_orders orders shown match that account with !totals_items items purchased and !totals_total spent:',
                     array('!totals_orders' => $totals['orders'],
                           '!totals_items' => $totals['items'],
                           '!totals_total' => uc_currency_format($totals['total']))) .'</p>'
           . theme('table', $header, $rows, array('width' => '100%', 'class' => 'uc-cust-orders-table'))
            .'<br />'. theme_pager(NULL, 50);

  return $output;
}

/**
 * Disable a country so it remains installed but is no longer selectable.
 */
function uc_country_disable($country_id) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = %d", $country_id);
  if (($country = db_fetch_object($result))) {
    if ($country->version > 0) {
      db_query("UPDATE {uc_countries} SET version = %d WHERE country_id = %d",
               0 - $country->version, $country_id);
      drupal_set_message(t('!country disabled.', array('!country' => $country->country_name)));
    }
    else {
      drupal_set_message(t('!country is already disabled.', array('!country' => $country->country_name)), 'error');
    }
  }
  else {
    drupal_set_message(t('Attempted to disable an invalid country.'), 'error');
  }
  drupal_goto('admin/store/settings/countries/edit');
}

/**
 * Enable a disabled country.
 */
function uc_country_enable($country_id) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = %d", $country_id);
  if (($country = db_fetch_object($result))) {
    if ($country->version < 0) {
      db_query("UPDATE {uc_countries} SET version = %d WHERE country_id = %d", abs($country->version), $country_id);
      drupal_set_message(t('@country enabled.', array('@country' => $country->country_name)));
    }
    else {
      drupal_set_message(t('@country is already enabled.', array('@country' => $country->country_name)), 'error');
    }
  }
  else {
    drupal_set_message(t('Attempted to enable an invalid country.'), 'error');
  }
  drupal_goto('admin/store/settings/countries/edit');
}

// Form to completely remove a country.
function uc_country_remove_form($form_state, $country_id) {
  // Fetch the country name from the database.
  $country = db_result(db_query("SELECT country_name FROM {uc_countries} WHERE country_id = %d", $country_id));

  // If orders exist for this country, show a warning message prior to removal.
  if ($_POST['op'] != t('Remove') && module_exists('uc_order')) {
    $count = db_result(db_query("SELECT COUNT(order_id) FROM {uc_orders} WHERE delivery_country = %d OR billing_country = %d", $country_id, $country_id));
    if ($count > 0) {
      drupal_set_message(t('Warning: @count orders were found with addresses in this country. Removing this country now will cause errors to show on those order pages. You might consider simply disabling this country instead.', array('@count' => $count)), 'error');
    }
  }

  // Store the country ID in the form array for processing.
  $form['country_id'] = array(
    '#type' => 'value',
    '#value' => $country_id,
  );

  return confirm_form($form, t('Are you sure you want to remove @country from the system?', array('@country' => $country)), 'admin/store/settings/countries/edit', NULL, t('Remove'));
}

function uc_country_remove_form_submit($form, &$form_state) {
  $country_id = $form_state['values']['country_id'];

  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = %d", $country_id);
  if (!($country = db_fetch_object($result))) {
    drupal_set_message(t('Attempted to remove an invalid country.'), 'error');
    drupal_goto('admin/store/settings/countries/edit');
  }

  db_query("DELETE FROM {uc_countries} WHERE country_id = %d", $country_id);
  db_query("DELETE FROM {uc_zones} WHERE zone_country_id = %d", $country_id);
  variable_del('uc_address_format_'. $country_id);

  $func_base = _country_import_include($country_id, $country->version);
  if ($func_base !== FALSE) {
    $func = $func_base .'_uninstall';
    if (function_exists($func)) {
      $func();
    }
  }

  drupal_set_message(t('!country removed.', array('!country' => $country->country_name)));
  drupal_goto('admin/store/settings/countries/edit');
}

/**
 * Update a country to its latest version.
 */
function uc_country_update($country_id, $version) {
  $result = db_query("SELECT * FROM {uc_countries} WHERE country_id = %d", $country_id);
  if (!($country = db_fetch_object($result))) {
    drupal_set_message(t('Attempted to update an invalid country.'));
    drupal_goto('admin/store/settings/countries');
  }

  if ($version < $country->version) {
    drupal_set_message(t('You cannot update to a previous version.'));
    drupal_goto('admin/store/settings/countries');
  }

  $func_base = _country_import_include($country_id, $version);
  if ($func_base !== FALSE) {
    $func = $func_base .'_update';
    if (function_exists($func)) {
      for ($i = $country->version; $i <= $version; $i++) {
        $func($i);
      }
    }
    db_query("UPDATE {uc_countries} SET version = %d WHERE country_id = %d", $version, $country_id);
    drupal_set_message(t('Country update complete.'));
  }
  else {
    drupal_set_message(t('Attempted to update an invalid country.'));
  }

  drupal_goto('admin/store/settings/countries');
}
