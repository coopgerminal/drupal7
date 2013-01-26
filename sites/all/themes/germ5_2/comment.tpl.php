<div class="art-Post" 	 >
    <div class="art-Post-body2">
<div class="art-Post-inner"  >

	<div class="comment<?php if ($comment->status == COMMENT_NOT_PUBLISHED) echo ' comment-unpublished'; ?>" margin-left="20px">
		

<div class="art-PostMetadataHeader">
		<div class="art-PostHeader2"  > 
			<img height="18" width="18" alt="" src="<?php echo get_full_path_to_theme(); ?>/images/PostCommentsIcon.png" class="art-metadata-icon" >
			<?php if ($title) {echo $title; } ?>

		</div>
		
		</div>
		
		<?php if ($submitted): ?>
			<span class="submitted"><?php echo $submitted; ?></span>
			<div class="cleared"></div>
		<?php endif; ?>	
		<?php if ($comment->new) : ?>
			<span class="new"><?php print drupal_ucfirst($new) ?></span>
		<?php endif; ?>
<div class="art-PostContent">
		
			<div class="art-article">
				<?php print $picture ?>
				<?php echo $content; ?>
			</div>

		</div>
		<div class="cleared"></div>
		
		<div class="links"><?php echo $links; ?><div class="cleared"></div></div>	
	</div>

</div>

    </div>
</div>
