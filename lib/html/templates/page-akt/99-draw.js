<script type='text/javascript'>
	google.charts.load('current', { packages: ['gauge', 'corechart', 'line', 'bar'], 'language': 'de' }).then(
		function () {
			setPVdata();
			setGauges();
			RTi = window.setInterval(RTimer,1000);
			drawChartData();
		});
</script>