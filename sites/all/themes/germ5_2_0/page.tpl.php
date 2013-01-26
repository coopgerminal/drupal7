<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php echo get_page_language($language); ?>" xml:lang="<?php echo get_page_language($language); ?>">

<head>
  <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" />  
  <?php echo $head; ?>
  <title><?php if (isset($head_title )) { echo $head_title; } ?></title>  
  <?php echo $styles ?>
  <?php echo $scripts ?>
  <!--[if IE 6]><link rel="stylesheet" href="<?php echo $base_path . $directory; ?>/style.ie6.css" type="text/css" /><![endif]-->  
  <!--[if IE 7]><link rel="stylesheet" href="<?php echo $base_path . $directory; ?>/style.ie7.css" type="text/css" media="screen" /><![endif]-->
  <script type="text/javascript">if (Drupal.jsEnabled) {$(document).ready(function(){
				window.setTimeout("artNoStyleAdding(document)", 2000);});}</script>
  <script type="text/javascript"><?php /* Needed to avoid Flash of Unstyle Content in IE */ ?> </script>
</head>

<body>

<div id="art-page-background-simple-gradient">
</div>
<div id="art-main">
<div class="art-Sheet">
    <div class="art-Sheet-tl"></div>
    <div class="art-Sheet-tr"></div>
    <div class="art-Sheet-bl"></div>
    <div class="art-Sheet-br"></div>
    <div class="art-Sheet-tc"></div>
    <div class="art-Sheet-bc"></div>
    <div class="art-Sheet-cl"></div>
    <div class="art-Sheet-cr"></div>
    <div class="art-Sheet-cc"></div>
    <div class="art-Sheet-body">
<div class="art-Header">
    <div class="art-Header-png"></div>
    <div class="art-Header-jpeg"></div>
<div class="art-Logo">
    <?php 
        if (!empty($site_name)) { echo '<h1 class="art-Logo-name"><a href="'.check_url($base_path).'" title = "'.$site_name.'">'.$site_name.'</a></h1>'; }
        if (!empty($site_slogan)) { echo '<div class="art-Logo-text">'.$site_slogan.'</div>'; }
    ?>
</div>
</div>
<?php if (!empty($navigation)): ?>
<div class="art-nav">
        <div class="l"></div>
    <div class="r"></div>
            <?php echo $navigation; ?>
	</div>
<?php endif;?>
<?php if (!empty($banner1)) { echo '<div id="banner1">'.$banner1.'</div>'; } ?>
<?php echo art_placeholders_output($top1, $top2, $top3); ?>
<div class="art-contentLayout">
<?php if (!empty($left)) echo '<div class="art-sidebar1">' . $left . "</div>"; 
else if (!empty($sidebar_left)) echo '<div class="art-sidebar1">' . $sidebar_left. "</div>";?>
<div class="<?php $l = null;
if (!empty($left)) $l = $left;
else if (!empty($sidebar_left)) $l=$sidebar_left;
$r = null;
if (!empty($right)) $r = $right;
else if (!empty($sidebar_right)) $r=$sidebar_right;          
echo art_get_content_cell_style($l, $r, $content); ?>">
<?php if (!empty($banner2)) { echo '<div id="banner2">'.$banner2.'</div>'; } ?>
<?php if ((!empty($user1)) && (!empty($user2))) : ?>
<table width="100%" cellpadding="0" cellspacing="0" border="0">
<tr valign="top"><td width="50%"><?php echo $user1; ?></td>
<td><?php echo $user2; ?></td></tr>
</table>
<?php else: ?>
<?php if (!empty($user1)) { echo '<div id="user1">'.$user1.'</div>'; }?>
<?php if (!empty($user2)) { echo '<div id="user2">'.$user2.'</div>'; }?>
<?php endif; ?>
<?php if (!empty($banner3)) { echo '<div id="banner3">'.$banner3.'</div>'; } ?>
<?php if (($is_front) || (isset($node) && isset($node->nid))): ?>              
<?php if (!empty($breadcrumb) || !empty($tabs) || !empty($tabs2)): ?>
<div class="art-Post">
    <div class="art-Post-body">
<div class="art-Post-inner">
<div class="art-PostContent">
<?php if (!empty($breadcrumb)) { echo $breadcrumb; } ?>
<?php if (!empty($tabs)) { echo $tabs.'<div class="cleared"></div>'; }; ?>
<?php if (!empty($tabs2)) { echo $tabs2.'<div class="cleared"></div>'; } ?>

</div>
<div class="cleared"></div>

</div>

    </div>
</div>
<?php endif; ?>
<?php if (!empty($mission)) { echo '<div id="mission">'.$mission.'</div>'; }; ?>
<?php if (!empty($help)) { echo $help; } ?>
<?php if (!empty($messages)) { echo $messages; } ?>
	
	
		<!-------------------------------------------------------------------------------------------------------------->					
		<!--------------------- Inici de la implementació específica dels blocs de la homepage  ------------------------>
		<!-------------------------------------------------------------------------------------------------------------->	
		<?php if ($node->nid)	echo art_content_replace($content); 	?>
				 				
        	 <?php if ($is_front) : ?>
	        	
	        	<?php if ($qAmbit == 'anonim') :?>			        	
			        		<?php  echo views_embed_view('vw_teaser_publica', 'page_1'); ?>
			        	
			      <?php else: ?>
					<!------------------------------------- FILA 1 AMB ELS AVISOS  ------------------------------------------------->					
	      		<div id="home1" >
								<?php 	        												
									$avisInfo = '';
	          				if ($qAmbit != 'anonim') 
	          					$avisInfo =  views_embed_view('viewbase', 'block_1', 'avis', 'socigerminal'); 									
									$blk1 = (object) array('subject' => 'Avisos urgents de Germinal', 'content' => $avisInfo, 'delta' => 'blk_avisos' );	    	      										
	          			echo theme('block', $blk1); 	          				
	        			?>	        		
			      </div>     	        		
          	<div  id="home2" >
          		<?php 
          		$avisInfo = '';
          			$avisInfo .=  views_embed_view('viewbase', 'block_1', 'avis', $qAmbit); 		  									
								$blk1 = (object) array('subject' => 'Avisos urgents de Germinal - ' . $qAmbit, 'content' => $avisInfo, 'delta' => 'blk_avisos' );	    	      	
          			echo theme('block', $blk1); 	      
          	    ?>      		
          	</div>	
          	 <div id="clearing"></div>	    
					
					<!------------------------------------- FILA 2 AMB LES NOTÍCIES  ------------------------------------------------->        
	        		<div id="home1" >
								<?php 	        												
									//S'obté la view amb la llista de notis de germinal i es temitza com a bloc
									$vwNotisGerm =  views_embed_view('viewbase', 'block_2', 'noticia', 'socigerminal'); 	
									
									//Enllaç de 'llegir més' que invoca una view en mode 'teaser'
									$vwNotisGerm .=  '<div class="GERMLLEGIRMES"><a title="title" href="' .$base_path . 'vw_teaser/noticia/socigerminal">Llistar-les totes</a></div>';
									
									//Es pinta la informació en forma de bloc
									$blk1 = (object) array('subject' => 'Informaci&oacute; de Germinal', 'content' => $vwNotisGerm, 'delta' => 'blk_notis' );	    	      	
	          			echo theme('block', $blk1); 	          				
	        			?>	        		
			      </div>	         	        		
	          	<div  id="home2" >
	          		<?php 
	          			//S'obté la view amb la llista de notis de l'ambit del soci i es temitza com a bloc
									$vwNotisAmbit =  views_embed_view('viewbase', 'block_2', 'noticia', $qAmbit); 	
									
									//Enllaç de 'llegir més' que invoca una view en mode 'teaser'
									$vwNotisAmbit .=  '<div class="GERMLLEGIRMES"><a title="title" href="' .$base_path . 'vw_teaser/noticia/' . $qAmbit .'">Llistar-les totes</a></div>';
									
									//Es pinta la informació en forma de bloc
									$blk12 = (object) array('subject' => 'Informaci&oacute; de Germinal - ' . $qAmbit, 'content' => $vwNotisAmbit, 'delta' => 'blk_notis');	          	
	          			echo theme('block', $blk12); 	 
	          	    ?>      		
	          	</div>	
	          	 <div id="clearing"></div>
						
	     		<!------------------------------------- FILA 3 AMB ELS ACTES PREVISTOS  ------------------------------------------------->	
	           	<div id="home1" >	           		
	           		<?php
	           			//Actes previstos a germinal
									$vwActesGerm =  views_embed_view('viewbase', 'block_3', 'acte', 'socigerminal'); 	
									//Es pinten en forma de bloc
									$blk1 = (object) array('subject' => 'Actes previstos a Germinal', 'content' => $vwActesGerm, 'delta' => 'blk_anuncis' );	    	      	
	          			echo theme('block', $blk1); 	          				
	           		?>		         	
							</div>							
	          	<div id="home2" >
	          		<?php
	           			//Actes previstos a germinal-qAmbit
									$vwActesAmbit =  views_embed_view('viewbase', 'block_3', 'acte', $qAmbit); 	
									$blk1 = (object) array('subject' => 'Actes previstos a Germinal - ' . $qAmbit, 'content' => $vwActesAmbit, 'delta' => 'blk_anuncis' );	    	      	
	          			echo theme('block', $blk1); 	          				
	           		?>	          		
	          	</div>	          	
	          	<div id="clearing"></div>
	     			
	     		<!------------------------------------- FILA 4 AMB ELS TAULELLS D'ANUNCIS  ------------------------------------------------->	
	     		<!-- FILA 4 documents, comissió-->		
	           	<div id="home1" >
	           		<?php	           	
	           			//Anuncis posats al taulell d'anuncis de germinal
									$vwAnuncisGerm =  views_embed_view('vw_taulell ', 'block_1', 'socigerminal'); 										
									$blk1 = (object) array('subject' => 'Tauler d&acute;anuncis de Germinal', 'content' => $vwAnuncisGerm, 'delta' => 'blk_documents' );	    	      	
	          			echo theme('block', $blk1); 	          				
	           		?>
							</div>
							<div id="home2" >
	          		<?php
									//Anuncis posats al taulell d'anuncis de germinal
									$vwAnuncisAmbit =  views_embed_view('vw_taulell ', 'block_1', $qAmbit); 										
									$blk1 = (object) array('subject' => 'Tauler d&acute;anuncis de Germinal - ' . $qAmbit, 'content' => $vwAnuncisAmbit, 'delta' => 'blk_documents' );	    	      		          	
	          		  echo theme('block', $blk1); 	          				
	           		?>
	          	</div>	          	
	          	<div id="clearing"></div>
	     			
	     			<?php endif; //is_front
	     			?> 
	     		<?php endif; ?>
	     		<!-------------------------------------------------------------------------------------------------------------->					
					<!--------------------- Final de la implementació específica dels blocs de la homepage  ------------------------>
					<!-------------------------------------------------------------------------------------------------------------->	
     			
     			
     			
     			
<?php else: ?>

<div class="art-Post">
    <div class="art-Post-body">
<div class="art-Post-inner">
<div class="art-PostContent">
<?php if (!empty($breadcrumb)) { echo $breadcrumb; } ?>
			
			<!-- LLP: Atenció : trec el títol en el formulari de creació de ct-base i ct-anunci (ja es posa per codi) -->
			<?php if (!empty($title) && !strstr($_GET['q'], 'add/ct-base') && !strstr($_GET['q'], 'add/ct-anunci')
									&& !strstr($_GET['q'], 'add/ct-entrada-bloc')): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>
			
<?php if (!empty($tabs)) { echo $tabs.'<div class="cleared"></div>'; }; ?>
<?php if (!empty($tabs2)) { echo $tabs2.'<div class="cleared"></div>'; } ?>
<?php if (!empty($mission)) { echo '<div id="mission">'.$mission.'</div>'; }; ?>
<?php if (!empty($help)) { echo $help; } ?>
<?php if (!empty($messages)) { echo $messages; } ?>

<!-- -------------------------------------------------------------------------------------------->
<!-- ORIGINAL: <?php /*echo art_content_replace($content);*/ ?> -->
<!---- LLP Inserció de codi que prèviament verifica que el perfil de l'usuari permet mostrar el contingut. -->
<?php
if ($perfilValid)
	echo art_content_replace($content);
else
	echo 'Acci&oacute; no permesa';
?>


</div>
<div class="cleared"></div>

</div>

    </div>
</div>
<?php endif; ?>



<?php if (!empty($banner4)) { echo '<div id="banner-4">'.$banner4.'</div>'; } ?>
<?php if (!empty($user3) && !empty($user4)) : ?>
<table width="100%" cellpadding="0" cellspacing="0" border="0">
<tr valign="top"><td width="50%"><?php echo $user3; ?></td>
<td><?php echo $user4; ?></td></tr>
</table>
<?php else: ?>
<?php if (!empty($user3)) { echo '<div id="user1">'.$user3.'</div>'; }?>
<?php if (!empty($user4)) { echo '<div id="user2">'.$user4.'</div>'; }?>
<?php endif; ?>
<?php if (!empty($banner5)) { echo '<div id="banner5">'.$banner5.'</div>'; } ?>
</div>
<?php if (!empty($right)) echo '<div class="art-sidebar2">' . $right . "</div>"; 
else if (!empty($sidebar_right)) echo '<div class="art-sidebar2">' . $sidebar_right . "</div>";?>

</div>
<div class="cleared"></div>
<?php echo art_placeholders_output($bottom1, $bottom2, $bottom3); ?>
<?php if (!empty($banner6)) { echo '<div id="banner6">'.$banner6.'</div>'; } ?>
<div class="art-Footer">
    <div class="art-Footer-inner">
        <?php echo art_feed_icon(url('rss.xml')); ?>
        <div class="art-Footer-text">
        <?php 
            if (!empty($footer_message) && (trim($footer_message) != '')) { 
                echo $footer_message;
            }
            else {
                echo '<p><a href="#">Contact Us</a>&nbsp;|&nbsp;<a href="#">Terms of Use</a>&nbsp;|&nbsp;<a href="#">Trademarks</a>&nbsp;|&nbsp;<a href="#">Privacy Statement</a><br />'.
                     'Copyright &copy; 2009&nbsp;'.$site_name.'.&nbsp;All Rights Reserved.</p>';
            }
        ?>
        <?php if (!empty($copyright)) { echo $copyright; } ?>
        </div>        
    </div>
    <div class="art-Footer-background"></div>
</div>

    </div>
</div>
<div class="cleared"></div>
<p class="art-page-footer"><?php echo t('Powered by ').'<a href="http://drupal.org/">'.t('Drupal').'</a>'.t(' and ').'<a href="http://www.artisteer.com/?p=drupal_themes">Drupal Theme</a>'.t(' created with ').'Artisteer'; ?>.</p>
</div>


<?php print $closure; ?>

</body>
</html>