
<!-- Un node de tipus ct-base només ha de ser visible a tothom si el node és de tipus 'public' o bé l'usuari està autenticat -->
<?php 
			if ($usuariLogat) :?>
			
	<div class="art-Post">
	    <div class="art-Post-body">
	<div class="art-Post-inner">
	<?php ob_start(); ?>
	<h2 class="art-PostHeader"> <?php echo art_node_title_output($title, $node_url, $page); ?>
	</h2>
	<?php if ($submitted): ?>
	<div class="art-PostHeaderIcons art-metadata-icons">
	<?php echo art_submitted_worker($date, $name); ?>
	
	</div>
	<?php endif; ?>
	<?php $metadataContent = ob_get_clean(); ?>
	<?php if (trim($metadataContent) != ''): ?>
	<div class="art-PostMetadataHeader">
	<?php echo $metadataContent; ?>
	
	</div>
	<?php endif; ?>
	<div class="art-PostContent">
	<div class="art-article"><?php print $picture; ?><?php echo $content; ?>
	<?php if (isset($node->links['node_read_more'])) { echo '<div class="read_more">'.get_html_link_output($node->links['node_read_more']).'</div>'; }?></div>
	</div>
	<div class="cleared"></div>
	<?php if (is_art_links_set($node->links) || !empty($terms)):
	$output = art_node_worker($node); 
	if (!empty($output)):	?>
	<div class="art-PostFooterIcons art-metadata-icons">
	<?php echo $output; ?>
	
	</div>
	<?php endif; endif; ?>
	
	</div>
	
	    </div>
	</div>

<?php else: ?>
		<div class="art-Post">
		 <div class="art-Post-body">
			<div class="art-Post-inner">			
			<?php echo t("No esteu autoritzat a visualitzar aquesta informaci&oacute");?>				
			</div>
		 </div>
		</div>
<?php endif; ?>