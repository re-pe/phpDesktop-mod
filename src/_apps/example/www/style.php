<style type="text/css">@import url("style.css");</style>
<a href="index.php">Go back to index</a>
| <a href="<?php echo $_SERVER["REQUEST_URI"];?>">Refresh</a>
<br><br>
<pre>
<?php 
	print_r(file_get_contents ("style.css"));
?>
</pre>