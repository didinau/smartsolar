<script type='text/javascript'>
	google.charts.load('current', {'packages':['gauge', 'corechart', 'line', 'bar'], 'language': 'de' });
	google.charts.setOnLoadCallback(ChartV);

	function ChartV() {
		var vdata=getVData();
		var options = {
			chart: {
				title: '',
				subtitle: '',
			},
			bars: 'vertical',
			bar: { groupWidth: '92%' },
			vAxis: {format: 'decimal'},
			height: 400,
			width: Lwidth ,
			legend: {position: 'none'},
			colors: _cCcomsumtion_,
		};
		var chart = new google.charts.Bar(get_el('chart_v'));
		chart.draw(vdata, google.charts.Bar.convertOptions(options));
	}
</script>
