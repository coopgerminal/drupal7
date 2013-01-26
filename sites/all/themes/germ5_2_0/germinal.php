<?php


/* Funcions implementades especificament pel theming de germinal */


/**
* Implementation of hook_theme.
* Register custom theme functions.
* LLP: Dins d'aquesta funció es registren els llocs on volem que es cridi el 'theming' específic a partir de l'id del 
			 formulari. En el nostre cas, ens interessa fer un theming especific pel formulari 'node_form', que és on es 
			 crea contingut pel site de germinal.
*/

function germ5_2_theme() 
{
  return array(
   /* // The form ID. (LLP: Això és per fer proves en el user profile form)
    'user_profile_form' => array(
      // Forms always take the form argument.
      'arguments' => array('form' => NULL),
    ),
    */
    'node_form' => array(
      // Forms always take the form argument.
      'arguments' => array('form' => NULL),
    ),
 
  )
  ;
}


   //* Theme override pel node
   //* The function is named themename_formid.
  function germ5_2_node_form($form) 
  {
 
	 	$output = '';
	 	$titForm = '';
	 	$subTitolForm = '';
	
		//De moment, només modifiquem el formulari si s'afegeix un node (si s'està en mode edició, deixem veure tots els camps)
		//(És així perquè he trobat alguns problemes si incloïem la edició dins de l'if) -LLP
		if (arg(1) =='add')
		{
 		
	 		if ($form['type']['#value'] == 'ct_base')
	 		{	 			
		  	//$output .= dsm($form); //LLP: Això és per mostrar al principi tots els elements del form aprofitant la funció 'dsm' de devel
		 
		
		 		//Problema: desapareix i el validador el demana!!
		 		//unset($form['field_tipus']); //Ens carreguem el camp tipus perquè ja hem passat el seu valor com a paràmetre
	 		
				//Es recullen els arguments en les corresponents variables
				$argTipus = arg(3);  //Argument 3 (tipus)
				
				//Valors d'inicialització necessaris
				$form['field_public']['value']['#value']  = 'No';	
				
				//D'entrada ocultem tots els camps
				$form['field_tipus']['value']['#type'] = 'hidden';  /// per a fer-la visible és 'select'
				$form['field_public']['value']['#type'] = 'hidden';
				$form['field_ambit']['value']['#type'] = 'hidden'; //IMPORTANT: no funciona si ataquem ['field_ambit']['#access'] !!!
				$form['field_comissio']['#access'] = false; //Això es pot fer quan el camp no és obligatori (i cal fer-ho quan el tipus és node)
				$form['field_dataini']['#access'] = false;
				$form['field_data_caducitat']['#access'] = false;
				
				//De moment deixem visible per a tots els tipus el field_imatge (només l'ocultarem en el cas de l'avís)
				//Idem el field_arxiu
								
				if (!empty($argTipus))
				{
						//Fixem el tipus i l'ocultem
					$form['field_tipus']['value']['#value'] = $argTipus; 
								
					//---- TIPUS NOTICIA -----
					if ($argTipus == 'noticia')
					{
							$argPublic = arg(4); //Argument 4 (Publica (si, no))
							$titForm = t("Creaci&oacute d'una not&iacute;cia ");
						
							if ($argPublic == 1)
							{
								$titForm .= t(' p&uacute;blica ');
								$form['field_public']['value']['#value']  = 'Si';							
								
							}
							else
								$titForm .= t(' per a Germinal ');
							//No caldria fer-lo visible, però per un Warning que dona Drupal "warning: mysqli_real_escape_string() expects parameter 2 to be string..." és necessari mostrar-lo
							$form['field_dataini']['#access'] = true; //Mostrem el selector de data inicial					
					}
					
					//---- TIPUS NOTICIA_COMISSIO -----
					if ($argTipus == 'noticia_comissio')
					{
						$argIdComissio = arg(6);
						$titForm = t("Creaci&oacute d'una informaci&oacute; associada a una comissi&oacute; per a Germinal");
						$subTitolForm = t("*IMPORTANT* : Recordeu de seleccionar en la corresponent casella la comissi&oacute; a la qual voleu vincular aquest contingut");
						
						//De moment, com que no sabem com lligar argIdComissio amb el combo de comissió, el deixem visible a l'usuari
						$form['field_comissio']['#access'] = true; 						
						//No cal veure l'àmbit... $form['field_ambit']['value']['#type'] = 'select';
						//No caldria fer-lo visible, però per un Warning que dona Drupal "warning: mysqli_real_escape_string() expects parameter 2 to be string..." és necessari mostrar-lo
							$form['field_dataini']['#access'] = true; //Mostrem el selector de data inicial						
					}
					
					//---- TIPUS AVÍS ---
					if ($argTipus == 'avis')
					{			
						$titForm = t("Creaci&oacute d'un av&iacute;s per a Germinal");										
				 		$form['body_field']['#access'] = false; //ocultem el body i l'inicialitzem a un valor buit
						$form['field_imatge']['#access'] = false; //Aquí no cal la imatge
						$form['field_arxiu']['#access'] = false; //Tampoc cal l'arxiu
						//Només quedarà visible el títol del ct_base com avís
						
						//Mostrem la data de caducitat
						$form['field_data_caducitat']['#access'] = true;
						//Cosa molt estranya: si no mostrem el camp data que no és la caducitat i que no necessitem, la caducitat no es guarda bé
						$form['field_dataini']['#access'] = true; //No tocar-ho!!
					}
					
					//---- TIPUS ACTE (Esdeveniment) ---
					if ($argTipus == 'acte')
					{
						$titForm = t("Creaci&oacute d'un esdeveniment per a Germinal");								
						$form['field_dataini']['#access'] = true; //Mostrem el selector de data inicial
					}
					
					//---- TIPUS DOCUMENT ---
					if ($argTipus == 'document')
					{
						$titForm = t("Creaci&oacute d'un document per a Germinal");								
						$form['field_imatge']['#access'] = false; //Aquí no cal la imatge
						//No caldria fer-lo visible, però per un Warning que dona Drupal "warning: mysqli_real_escape_string() expects parameter 2 to be string..." és necessari mostrar-lo
						$form['field_dataini']['#access'] = true; //Mostrem el selector de data inicial		
					}
								
					
					//-------------------------------------
					//Camps/arguments comuns				
					//qAmbit
					$argAmbit = arg(5);  //Argument 5 (qAmbit)
					if (!empty($argAmbit))
					{
						if ($argAmbit != 'socigerminal' && ($argTipus == 'noticia' || 
																								$argTipus == 'avis' || 
																								$argTipus == 'acte' ||
																								$argTipus == 'document' || 
																								$argTipus == 'noticia_comissio'
																								) )//Només per alguns tipus mostrem l'ambit
							$titForm .= ' - ' . $argAmbit;
						
						$form['field_ambit']['value']['#value']  = $argAmbit;						
					}
					//-------------------------------------					
					
				}//////////// Endif  (!empty($argTipus))
			
			} ///////////// Endif ct_base
		}



		//Modificació del formulari d'edició d'un perfil de manera que només es vegin els camps que no són ni titol ni body
		if ($form['type']['#value'] == 'ct_perfil')
 		{
				$form['title']['#type'] = 'hidden';
				$form['title']['#value'] = '.'; //cal que #value tingui alguna cosa, és requerit!
				$form['body_field']['#access'] = false; 
 		}


		//Modificació del formulari d'edició d'un anunci de manera que només es vegin els camps que no són ni titol ni body
		if ($form['type']['#value'] == 'ct_anunci'  && 	arg(1) == 'add') 
 		{
 				$titForm = t("Publicar un anunci al tauler de Germinal");
				
				//Posem el valor de l'argument ambit en cas que s'hagi especificat
				$argAmbit = arg(3);  //En aquest tipus de contingut, l'argument 3 és qAmbit
				if (!empty($argAmbit))
				{
					if ($argAmbit != 'socigerminal' )//Només per alguns tipus mostrem l'ambit
						$titForm .= ' - ' . $argAmbit;
					
					$form['field_ambit']['value']['#type'] = 'hidden';
					$form['field_ambit']['value']['#value']  = $argAmbit;						
				}
 		}

 		//Modificació del formulari d'edició d'un anunci de manera que només es vegin els camps que no són ni titol ni body
		if ($form['type']['#value'] == 'ct_entrada_bloc'  && 	arg(1) == 'add') 
 		{
 				$argIdBitacora = arg(6);  //Argument 5 (qAmbit)
 				$nodeBtc = node_load($argIdBitacora);
 				if ($nodeBtc != null)
 				{
 					  //NOTA: cal tocar lleuregament page.tpl per ocultar el title en cas de ct_entrada_bloc tal i com es fa en ct-base
 						$titForm = t("Nova entrada al bloc: ") . $nodeBtc->title;
 						$form['field_bitacora']['#access'] = false; //Ocultem la combo amb la bitacora a la que pertany l'entrada
 				}
 		}

		
		//Es pinta el títol del formulari a dalt perquè l'usuari sàpiga què està creant
		//Això està en conjunció amb el fet que dins de page.tpl.php forcem que el títol de la pàgina sigui buit en cas de 
		//creació d'un node de tipus ct-base (... --> !strstr($_GET['q'], 'add/ct-base')....
		$output = "<h2>". $titForm . "</h2>";
	
		if ($subTitolForm)
			$output .= "<h6>". $subTitolForm . "</h6>";
	
 		$output .= drupal_render($form);
 		
 		//Tornem a posar els botons de guardar/cancel·lar al final del formulari, doncs sembla que pel fet d'utilitzar 
 		//camps del CCK, els botons originals queden pel mig del formulari. Almenys així assegurem que també seran al final
 		//(per fer-ho bé, caldria evitar els botons originals)
 			if ($form['type']['#value'] == 'ct_base' || $form['type']['#value'] == 'ct_anunci')
 				$output .= theme_item($form['buttons']);
 		 
  	return $output;
  }

/*
   //* Theme override for user edit form.
   //* The function is named themename_formid.
  function germ5_2_user_profile_form($form) {
    $output = '';
  // Print out the $form array to see all the elements we are working with.
  $output .= dsm($form); //LLP: Això és per mostrar al principi tots els elements del form aprofitant la funció 'dsm' de devel
  // Once I know which part of the array I'm after we can change it.

  // You can use the normal Form API structure to change things, like so:
  // Change the Username field title to Login ID.
  //$form['account']['name']['#title'] = t('Titol modificat');
  
  // PIX: EUREKA, funciona
  //unset($form['account']['mail']); //Ens carreguem el camp 'mail'
  
  // Make sure you call a drupal_render() on the entire $form to make sure you
  // still output all of the elements (particularly hidden ones needed
  // for the form to function properly.)
  $output .= drupal_render($form);
  return $output;
  }
*/


//**************************************** FUNCIONS AFEGIDES LLP ************************************

function obte_ambit($user)
{
	$qAmbit = "anonim";

		if(in_array('germinal_sants', $user->roles)) 
		{
			$qAmbit = "sants";
		} 
		else 
		{
			if(in_array('germinal_farro', $user->roles)) 
			{
				$qAmbit = "farro";
			}  
			else
			{
				if(in_array('germinal_sarria', $user->roles)) 
				{
					$qAmbit = "sarria";
				}  
				else
				{
					if(in_array('germinal_poble_sec', $user->roles)) 
					{
						$qAmbit = "poblesec";
					}  
					else
					{
						if(in_array('germinal_valles', $user->roles)) 
						{
							$qAmbit = "rubi";
						}  
						else
						{
							//Si no té cap dels rols de centre, automàticament busquem si li atorguem almenys el rol d'usuari autenticat
							if(in_array('authenticated user', $user->roles)) 
							{
								$qAmbit = "socigerminal";
							}  			
						}
					}
				}
			}
		} 
		return $qAmbit;	
}



function obte_id_comissio($infoperf)
{
  $idComissio = $infoperf->field_comissio_ref[0]['nid'];
  
  //Per assegurar que aquesta variable imposarà un valor impossible de comissió com a argument del view, posem -1 si no n'hi ha
	if (empty($idComissio))	
			$idComissio = -1;
			
	return $idComissio;	
}

function obte_id_comissio2($infoperf)
{
  $idComissio2 = $infoperf->field_comissio_ref[1]['nid'];
  
  //Per assegurar que aquesta variable imposarà un valor impossible de comissió com a argument del view, posem -1 si no n'hi ha
	if (empty($idComissio2))	
			$idComissio2 = -1;
			
	return $idComissio2;	
}

function obte_id_comissio3($infoperf)
{
  $idComissio3 = $infoperf->field_comissio_ref[2]['nid'];
  
  //Per assegurar que aquesta variable imposarà un valor impossible de comissió com a argument del view, posem -1 si no n'hi ha
	if (empty($idComissio3))	
			$idComissio3 = -1;
			
	return $idComissio3;	
}



function obte_nom_comissio($idComissio)
{
  $usrComissio =  node_load($idComissio); 	
	$nomComissio = $usrComissio->title;
	return $nomComissio;
}

function obte_variables_page_node(&$vars)
{

		//-- 1 -- creem la variable que conté l'ambit o germinalet de l'usuari i si és logat (autenticat) o no
		$vars['qAmbit'] = obte_ambit($vars['user']);
	
		$vars['usuariLogat'] = user_is_logged_in();
		
		//-- 2 -- creem les variables amb l'id i el nom de la comissió a la qual pertany l'usuari
		$infoperf = content_profile_load('ct_perfil', $vars['user']->uid);
			
		$vars['idComissio'] = obte_id_comissio($infoperf);
		$vars['idComissio2'] = obte_id_comissio2($infoperf);
		$vars['idComissio3'] = obte_id_comissio3($infoperf);
		
		$vars['nomComissio'] = obte_nom_comissio($vars['idComissio']);
		$vars['nomComissio2'] = obte_nom_comissio($vars['idComissio2']);
		$vars['nomComissio3'] = obte_nom_comissio($vars['idComissio3']);
		
		//-- 3 -- obtenim l'array que conté els ids de bitacora que l'usuari té permís via perfil
		$vars['arBlocsUser'] = array();
		for ($i = 0; $i < count($infoperf->field_bitacora_ref); $i++) 
		{
			//$var[] equival a fer push()
	  	$vars['arBlocsUser'][] = $infoperf->field_bitacora_ref[$i]['nid'];
}

		//-- 4 --  Construcció dels arrays que recullen els permisos dels blogs o bitàcores a nivell global 
		//				 (tothom pot escriure, només qui té el perfil, etc)...
		construeixArrayPermisosBlogs($vars);
}


function reescriu_enllas_menu(&$item, $vars)
{

	//Canvi del codi pel valor de la variable corresponent
	//També es canvia el títol perquè aparegui en l'opció de menú que veu l'usuari
	
	//qAmbit
	$item['link']['href'] = str_replace("qAmbit", $vars['qAmbit'], $item['link']['href']);
	$item['link']['title'] = str_replace("qAmbit", $vars['qAmbit'], $item['link']['title']);
	
	//qIdComissio1
	if ($vars['idComissio1'] != -1)
	{
		$item['link']['href'] = str_replace("qIdComissio1", $vars['idComissio'], $item['link']['href']);	
		$item['link']['title'] = str_replace("qNomComissio1", $vars['nomComissio'], $item['link']['title']);
	}
	
	//qIdComissio2
	if ($vars['idComissio2'] != -1)
	{
		$item['link']['href'] = str_replace("qIdComissio2", $vars['idComissio2'], $item['link']['href']);	
		$item['link']['title'] = str_replace("qNomComissio2", $vars['nomComissio2'], $item['link']['title']);
	}
	//qIdComissio3
	if ($vars['idComissio3'] != -1)
	{
		$item['link']['href'] = str_replace("qIdComissio3", $vars['idComissio3'], $item['link']['href']);	
		$item['link']['title'] = str_replace("qNomComissio3", $vars['nomComissio3'], $item['link']['title']);
	}
	
	//ocultem l'opció de menú si encara conté la coficicació  qIdComissioX
	if (strstr( $item['link']['href'], "qIdComissio1" ) || 
			strstr( $item['link']['href'], "qIdComissio2" ) ||
			strstr( $item['link']['href'], "qIdComissio3" ))
	{
		$item['link']['hidden'] = 1;
		$item['link']['access'] = false;
	}
	
	//S'invoca la funció validaNovaEntradaBloc() que verifica que no es tracti d'una entrada de bloc
	//pel qual el perfil de l'usuari no té permisos. En aquest cas, retorna fals i s'oculta l'opció
	if (!validaNovaEntradaBloc($item['link']['href'], $vars))
	{
			$item['link']['hidden'] = 1;
			$item['link']['access'] = false;
	}
		
	
	//Iteració per tots els fills de l'item 
	$children = &$item['below'];
	if ($children)
	{
			foreach($children as $key => &$mi) 
				reescriu_enllas_menu($mi, $vars);
	}	
}

//Retorna true si l'string no correspon a una entrada de bloc o bé hi correspon però l'id de bloc o bitàcora que
//es vol crear està dins de l'array de blocs permesos $vars['arBlocsUser']
function validaNovaEntradaBloc($qs, $vars)
{

		$addbloc = 'add/ct-entrada-bloc/'; // Falta  '/0/0/0/' i a continuació ve l'id del bloc

		$pos = strpos($qs, $addbloc);
		if (!($pos === false)) //Si trobem la cadena, només permetrem la validesa si l'Id del tipus de bloc del qual volem afegir una entrada coincideix amb el perfil de l'usuari de blocs permesos
		{
			$posParams = $pos + strlen($addbloc);
			$params = substr($qs, $posParams);
			$arParams = explode('/', $params);
			
			//El quart paràmetre és el que té l'ID del bloc i ha d'estar dins arBlocsUser si l'usuari té seleccionat aquell bloc en el seu perfil
			$idBloc = $arParams[3];
			
			//Si el bloc no està en l'array arBlocsPermEsc o arBlocsPermLectEsc significa que es pot escriure en aquest bloc tranquil·lament
			if (!in_array($idBloc, $vars['arBlocsPermEsc']) &&  !in_array($idBloc, $vars['arBlocsPermLectEsc']) ) 
				return true;
			
			//Si l'usuari no té blocs referenciats, no es valida
			if (empty($vars['arBlocsUser']) || count($vars['arBlocsUser']) == 0) 
				return false;
			
			//Es retorna fals si l'usuari no té idBloc en el seu perfil
			if (!in_array($idBloc, $vars['arBlocsUser'])) 
				return false;
				
			return true;
		}
		else
		{

			$readBloc = 'vw_bloc/';	
			$pos = strpos($qs, $readBloc);
			if (!($pos === false)) 
			{
				//Si trobem la cadena que indica lectura d'un bloc, només permetrem la validesa si l'idBloc 
				//no està dins l'array arBlocsPermLectEsc o bé, en cas que hi estigui, si l'usuari té el perfil assignat a aquell bloc
				$posParams = $pos + strlen($readBloc);
				$params = substr($qs, $posParams);
			
				//El primer paràmetre és el que té l'ID del bloc 
				$idBloc = (int)$params;

				//Si el bloc no està en l'array arBlocsPermLectEsc significa que es pot llegir en aquest bloc tranquil·lament
				if ( !in_array($idBloc, $vars['arBlocsPermLectEsc']) ) 
					return true;
				
				//Si l'usuari no té blocs referenciats, no es valida
				if (empty($vars['arBlocsUser']) || count($vars['arBlocsUser']) == 0) 
				return false;
				
				//Es retorna fals si l'usuari no té idBloc en el seu perfil
				if (!in_array($idBloc, $vars['arBlocsUser'])) 
				return false;
						
			}
		}
		
		return true;
}


//Hi ha alguns casos on no és suficient el sistema de permisos de Drupal, sobretot quan volem garantir algunes 
//funcionalitats que es basen en el perfil de l'usuari (PerfilGerminal). Per això, definim una variable $perfilValid
//a la qual se li dóna valor en aquesta funció però que es consulta en la page.tpl. Si té valor fals, es mostra un
//missatge que no hi ha els permisos adequats
function decideixValidesaPerfil(&$vars)
{
		//1) Si estem creant o editant una entrada de bloc, només es permetrà si el Perfil té assignat el tipus de blog
		$qs = $_GET['q'];
		$vars['perfilValid'] = validaNovaEntradaBloc($qs, $vars);
}


function construeixArrayPermisosBlogs(&$vars)
{
	//Consulta dels diferents modes de cada bitacora, construcció dels array  i emmagatzemament en variables 
	$result = db_query('SELECT n.nid FROM {node} n WHERE n.type = '."'bitacora'".' and n.status = 1');
	$vars['arBlocsPermEsc'] = array();
	$vars['arBlocsPermLectEsc'] = array();
	
  while ($node = db_fetch_object($result))
  {
     $nodeBit = node_load($node->nid);
     $val = $nodeBit->field_mode[0]['value'];
     
     if ($val == 1) //Tipus de blog: soci requereix permisos per escriure
         $vars['arBlocsPermEsc'][] = $node->nid;
     else
     	if ($val == 2)//Tipus de blog: soci requereix permisos per escriure i per llegir
     	   $vars['arBlocsPermLectEsc'][] = $node->nid;
  }
    
	
}

//Reescriptura de la funció que permet crear noves variables utilitzables dins de template.php
function germ5_2_preprocess_page(&$vars) 
{
		obte_variables_page_node($vars);
	
		//-- 3 -- Reescrivim aquelles opcions de menú que porten alguna variable codificada segons el valor correcte de la variable	
  	//Obtenim l'estructura de dades corresponent al menú que hem creat anomenat 'menu-superior' (cal posar prefix 'menu-')
  	$tree = menu_tree_page_data('menu-menu-principal'); 															
	
		foreach($tree as $key => &$mi) //Observem com amb '&' s'agafa la referència de l'objecte
			reescriu_enllas_menu($mi, $vars);
			
						
		//-- 4 -- Guardem a la variable  'navigation' el menú superior reescrit de forma que els enllaços que ho necessitin continguin el germinalet
  	//NOTA: Desactivat pel tema de l'Enric que no suporta el menú superior desplegable d'artisteer
  	//NOTA: $vars['navigation'] (per a menú Artisteer desplegable) ho canviem per  $vars['menuintern'] 
  	//$vars['menu_intern'] =  art_navigation_links_worker(menu_tree_output($tree), true); 
  	 $vars['navigation'] =  art_navigation_links_worker(menu_tree_output($tree), true); 
  	
		
		//-- 5 -- De moment ens carreguem el $breadcrumb perquè no acaba de sortir bé. Segurament el podríem muntar a través de 
		//				l'opció de menú que s'ha polsat...
		$vars['breadcrumb'] = '';
		
		//-- 6 -- Si no es tracta d'un anònim, concatenem el centre al nom del site
		if ($vars['qAmbit'] != 'anonim')
			$vars['site_name'] .= ' - ' . $vars['qAmbit'];
			
		//-- 7 -- Decisió del valor de la variable $perfilValid que si està a fals impedirà que es vegi el formulari o el que sigui a la pàgina
		
		$vars['perfilValid'] = true;
		decideixValidesaPerfil($vars);

}


function germ5_2_preprocess_node(&$vars) 
{
	
	obte_variables_page_node($vars);
	
}



