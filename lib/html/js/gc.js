function tickerarray(max,step=2500,min=0,suffix=false)
{
	ticker=new Array();
	for(let i=min;i<max;i+=step)
		if(suffix)
			ticker[ticker.length]=i+""+suffix;
		else
			ticker[ticker.length]=i;
	ticker[ticker.length]=max;
	return ticker;
}

function uarr(m)
{
	back=new Array();
	s=0;
	argc=(arguments.length>m ? m : arguments.length)+1;
	for (let i = 1; i < argc; i++)
	{
		//console.log("argc="+argc+" - back"+s,arguments[i]);
		back[s++]=arguments[i];
	}
	return back;
}

function RTimer()
{
	var now=new Date().getTime();

	//console.log(now,"/"+RTiEx);


	if(now > RTiEx)
	{
		RTiEx=0 + now + conf.refreshAktuell;
		doRefresh();

		//console.log("maxpid/lmaxpid",maxpid+"/"+lmaxpid);
		if(maxpid>lmaxpid)
		{
			lmaxpid=maxpid;
			drawChartData();
		}
	}
}


function reloadConf()
{
	confscr = document.createElement('script');
	confscr.src = "?mode=nconf";
	confscr.onload = function(){
		console.log("Conf",confid);
	};
	document.head.appendChild(confscr);
}


function doRefresh()
{
	//console.log("Refresh","nn");
	var aj=$.ajax({
		url:"?mode=pdata",
		dataType: 'json',
		async: false}).responseText;

	var R=jQuery.parseJSON(aj);

	console.log("doRefresh","");
	//console.log("R:",R);

	if( R[0].configid )
	{
		if( R[0].configid != confid )
		{
			reloadConf();
			confid=R[0].configid;
		}
	}

	if(R[0].maxpid)
		maxpid=R[0].maxpid;

	set_text("progsonne",conf.txt.prognose+" "+R[0].dprognoseF+" "+conf["bez"]["kwh"]+", "+conf.txt.tomorrow+" "+R[0].dprognextdF+" "+conf["bez"]["kwh"]+", "+conf.txt.sun+": "+R[0].sonnenaufgang+" - "+R[0].sonnenzenit+" - "+R[0].sonnenuntergang+" / "+R[0].sonnenscheindauer);

	set_text("txtDate",conf.txt.timestamp+" "+R[0].dataTime+" ("+R[0].x+")");


//console.log("doRefresh",R.length);

	for(i=1;i<R.length;i++)
	{
		var aktdev=R[i];
		var aktsn=R[i].pdevice;

		set_text("PZ"+aktsn,aktdev.pzeit);

		//console.log(aktsn, aktdev);

// Generator
	if(conf["generators"][aktsn])
		if(conf["generators"][aktsn]["showdevice"])
		{
			if(conf.pv1gauge)
			{
				coG["PV1"+aktsn].setVal(aktdev.ppv1);
			}else{
				var pw=aktdev.ppv1 / 40;	//wegen max. 4000 Watt
				var pw=aktdev.ppv1 * 100 / (conf["generators"][aktsn]["powermppt1"]?conf["generators"][aktsn]["powermppt1"]:4000);
				var e=new myBargraph("mppt1"+aktsn,pw,conf.color.barneutral,conf.color.BChartBGColor,"PV1: ",Math.round(aktdev.ppv1)+" "+conf["bez"]["w"]);
				e.draw();
			}

			if(conf["generators"][aktsn]["mppts"]>1)
				if(conf.pv2gauge)
				{
					coG["PV2"+aktsn].setVal(aktdev.ppv2);
				}else{
					var pw=aktdev.ppv2 * 100 / (conf["generators"][aktsn]["powermppt2"]?conf["generators"][aktsn]["powermppt2"]:4000);
					var e=new myBargraph("mppt2"+aktsn,pw,conf.color.barneutral,conf.color.BChartBGColor,"PV2: ",Math.round(aktdev.ppv2)+" "+conf["bez"]["w"]);
					e.draw();
				}

			if(conf["generators"][aktsn]["mppts"]>2)
				if(conf.pv3gauge)
				{
					coG["PV3"+aktsn].setVal(aktdev.ppv3);
				}else{
					var pw=aktdev.ppv3 * 100 / (conf["generators"][aktsn]["powermppt3"]?conf["generators"][aktsn]["powermppt3"]:4000);
					var e=new myBargraph("mppt3"+aktsn,pw,conf.color.barneutral,conf.color.BChartBGColor,"PV3: ",Math.round(aktdev.ppv3)+" "+conf["bez"]["w"]);
					e.draw();
				}

			if(conf["generators"][aktsn]["mppts"]>3)
				if(conf.pv4gauge)
				{
					coG["PV4"+aktsn].setVal(aktdev.ppv4);
				}else{
					var pw=aktdev.ppv4 * 100 / (conf["generators"][aktsn]["powermppt4"]?conf["generators"][aktsn]["powermppt4"]:4000);
					var e=new myBargraph("mppt4"+aktsn,pw,conf.color.barneutral,conf.color.BChartBGColor,"PV4: ",Math.round(aktdev.ppv4)+" "+conf["bez"]["w"]);
					e.draw();
				}
			if(conf["generators"][aktsn]["battery"])
			{
				if(aktdev.ibat>0)
				{
					bi=conf["generators"][aktsn]["maxchargepower"]?conf["generators"][aktsn]["maxchargepower"]:10000;
					pw=aktdev.pcharge / bi * 100;
					bi=aktdev.pchargeF+" "+conf["bez"]["w"]+" / "+aktdev.ibatF+conf["bez"]["a"];
				}else{
					pw=0;
					bi=conf.txt.none;
				}
				var e=new myBargraph("charger"+aktsn,pw,conf.color.bargreen,conf.color.BChartBGColor,conf.txt.batcharge+": "+bi);
				e.draw();
			}


			if(coG["PV"+aktsn])
				coG["PV"+aktsn].setVal(aktdev.ppv);

			set_text("SN"+aktsn,aktsn);
			set_text("dpvF"+aktsn,aktdev.dpvF);
			set_text("dpvFbez"+aktsn,conf["bez"]["kwh"]);
			set_text("gPZ"+aktsn,aktdev.pzeit);

			if(aktdev.mdpv>0)
			{
				var pw=aktdev.dpv * 100 / aktdev.mdpv;
				var e=new myBargraph("maxSolarBE"+aktsn,pw,conf.color.bargreen,conf.color.BChartBGColor,
					Math.round(pw)+"% "+conf.txt.from+" ","max. "+aktdev.mdpvF+conf["bez"]["kwh"]);
				e.draw();
			}

		}

// Battery
	if(conf["batteries"][aktsn])
		if(conf["batteries"][aktsn]["showdevice"])
		{
			if(conf.batgauge)
			{
				coG["Bat"+aktsn].setVal(aktdev.soc);
			}else{
				aminsoc=false;
				if (conf["batteries"][aktsn]["minsoc"])
					aminsoc=conf["batteries"][aktsn]["minsoc"];
				if (aktdev.more.minsoc)
					aminsoc=aktdev.more.minsoc;

//console.log("aktdev"+aktsn,"minsoc "+aminsoc);

				if (aminsoc)
					aktdev.socpercent=Math.round((aktdev.soc -aminsoc) / (100 - aminsoc) * 100);
				else
					aktdev.socpercent=aktdev.soc;

				var e=new myBargraph("batsoc"+aktsn,aktdev.socpercent,conf.color.barblue,conf.color.BChartBGColor,
					conf.txt.battery+": ",aktdev.socpercent+"%");
				if(aktdev.pcharge>0) e.markercolor=conf["color"]["bargreen"];
				if(aktdev.pdischarge>0) e.markercolor=conf["color"]["barred"];
				e.draw();
			}
			coG["Laden"+aktsn].setVal( aktdev.pcharge - aktdev.pdischarge);

			/*if(aktdev.more)
			{
				console.log(aktsn, JSON.parse(aktdev.more));
			}*/


			set_text("socF"+aktsn,aktdev.socF);
			set_text("socFbez"+aktsn,conf["bez"]["%"]);
			set_text("vbatF"+aktsn,aktdev.vbatF);
			set_text("vbatFbez"+aktsn,conf["bez"]["v"]);
			set_text("ibatF"+aktsn,aktdev.ibatF);
			set_text("ibatFbez"+aktsn,conf["bez"]["a"]);
			set_text("bPZ"+aktsn,aktdev.pzeit);

		}

// Inverter
	if(conf["inverters"][aktsn])
		if(conf["inverters"][aktsn]["showdevice"])
		{
			if(conf["inverters"][aktsn]["meter"])
			{
				if(conf.exportgauge)
					coG["Export"+aktsn].setVal(aktdev.pgrid);
				else{
					if(conf.barBezEinspKombi==0)
					{
						var pw=aktdev.pgrid / 50;	//wegen max. 5.000 Watt
						var e=new myBargraph("lexport"+aktsn,pw,conf["color"]["bargreen"],conf.color.BChartBGColor,
							conf.txt.gridtgrid+": ",aktdev.pgrid+" "+conf["bez"]["w"]);
						e.draw();
					}
				}
				if(conf.lastgauge)
					coG["Last"+aktsn].setVal(aktdev.pload);
				else{
					var pw=aktdev.pload / 100;	//wegen max. 10.000 Watt
					var e=new myBargraph("llast"+aktsn,pw,conf["color"]["barred"],conf.color.BChartBGColor,
						conf.txt.load+": ",aktdev.pload+" "+conf["bez"]["w"]);
					e.draw();
				}

				if(conf.bezuggauge)
					coG["Bezug"+aktsn].setVal(aktdev.puser);
				else{
					if(conf.barBezEinspKombi==0)
					{
						var pw=aktdev.puser / 100;	//wegen max. 10.000 Watt
						var e=new myBargraph("lbezug"+aktsn,pw,conf["color"]["barred"],conf.color.BChartBGColor,
							conf.txt.gridfrgrid+": ",aktdev.puser+" "+conf["bez"]["w"]);
						e.draw();
					}
				}

				if(conf.barBezEinspKombi)
				{
					if(aktdev.pgrid)
					{
						var pw=aktdev.pgrid / 50;	//wegen max. 5.000 Watt
						var e=new myBargraph("bezeinkombi"+aktsn,pw,conf["color"]["bargreen"],conf.color.BChartBGColor,
							conf.txt.gridtogrid+": ",aktdev.pgrid+" "+conf["bez"]["w"]);
					}else{
						var pw=aktdev.puser / 100;	//wegen max. 10.000 Watt
						var e=new myBargraph("bezeinkombi"+aktsn,pw,conf["color"]["barred"],conf.color.BChartBGColor,
							conf.txt.gridfrgrid+": ",aktdev.puser+" "+conf["bez"]["w"]);
					}
					e.draw();
				}

				if(conf["inverters"][aktsn]["battery"])
				{
					var bi=0;
					var pw=0;
					if(aktdev.pdischarge>0)
					{
						bi=aktdev.pdischarge / conf["inverters"][aktsn]["maxdischargepower"] * 100;
						pw=conf.txt.batcurrent+": "+aktdev.pdischargeF+" "+conf["bez"]["w"]+" / "+aktdev.ibatF+conf["bez"]["a"];
					}else{
						bi=0;
						pw=conf.txt.batnouse;
					}
					var e=new myBargraph("libat"+aktsn,bi,conf["color"]["barred"],conf.color.BChartBGColor,pw,"");
					e.draw();

					bi=aktdev.paccharge / 1000 * 100;
					pw=conf.txt.batchfrgr+": "+aktdev.pacchargeF+" "+conf["bez"]["w"];
					var e=new myBargraph("lgridcharge"+aktsn,bi,conf["color"]["barred"],conf.color.BChartBGColor,pw,"");
					e.draw();
				}
			}
			set_text("iPZ"+aktsn,aktdev.pzeit);

		set_text("txtPowerT"+aktsn,"<table width='100%'>"+
			"<tr><td class='aleft'><b>"+conf.txt.togrid+"</b></td><td class='aright'>"+aktdev.dgridF+"</td><td class='aleft'>"+conf["bez"]["kwh"]+"</td></tr>"+
			"<tr><td class='aleft'><b>"+conf.txt.consumption+"</b></td><td class='aright' width='1'>"+aktdev.dloadF+
				"</td><td class='aleft' width='1'>"+conf["bez"]["kwh"]+"</td></tr>"+
			"<tr><td class='aleft'>"+conf.txt.partfrgrid+" "+aktdev.dbezugpF+conf["bez"]["%"]+"</td><td class='aright'>"+aktdev.dfromgridF+
				"</td><td class='aleft'>"+conf["bez"]["kwh"]+"</td></tr>"+
			(aktdev.dgridc>0? "<tr><td class='aleft'>"+conf.txt.fee+"</td><td class='aright'>"+aktdev.dgridcF+"</td><td class='aleft'>"+conf["bez"]["cent"]+"</td></tr>":"")+
			"</table>");

//			"<tr><td class='aleft'><b>Systemproduktion</b></td><td class='aright'>"+aktdev.dproduktionF+"</td><td class='aleft'>"+conf["bez"]["kwh"]+"</td></tr>"+
//			"<tr><td class='aleft'>davon selbst "+aktdev.dselbstpF+conf["bez"]["%"]+"</td><td class='aright'>"+aktdev.dselbstF+"</td><td class='aleft'>"+conf["bez"]["kwh"]+"</td></tr>"+

/*
			set_text("ddischargeF"+aktsn,aktdev.ddischargeF);
			set_text("ddischargeFbez"+aktsn,conf["bez"]["kwh"]);
			set_text("dchargeF"+aktsn,aktdev.dchargeF);

console.log("dchargeF"+aktsn, aktdev.dchargeF);

			set_text("dchargeFbez"+aktsn,conf["bez"]["kwh"]);
			set_text("dgridchargeF"+aktsn,aktdev.dgridchargeF);
			set_text("dgridchargeFbez"+aktsn,conf["bez"]["kwh"]);
*/

			/*if(aktdev.more)
			{
				console.log(aktsn, JSON.parse(aktdev.more));
			}*/

		}

		set_text("ddischargeF"+aktsn,aktdev.ddischargeF);
		set_text("ddischargeFbez"+aktsn,conf["bez"]["kwh"]);
		set_text("dchargeF"+aktsn,aktdev.dchargeF);

		//console.log("dchargeF"+aktsn, aktdev.dchargeF);

		set_text("dchargeFbez"+aktsn,conf["bez"]["kwh"]);
		set_text("dgridchargeF"+aktsn,aktdev.dgridchargeF);
		set_text("dgridchargeFbez"+aktsn,conf["bez"]["kwh"]);




	}
}

function setGauges()
{
	for(let dev of Object.keys(conf["batteries"]))
	{
		if(conf["batteries"][dev]["showdevice"])
		{
			coG["Bat"+dev]= new sGauge("chart_Bat"+dev,"S",0,100,['0','20','40','60','80', '100'],4,{ suffix: '%', fractionDigits: 0 },[ ['Label', 'Value'], [conf.txt.battery, 1] ]);
			coG["Laden"+dev]= new sGauge("chart_Laden"+dev,"S",-3000,3000,['-3kW','-2kW','-1kW','0','1kW','2kW','3kW'],4,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], [conf.txt.status, -3000 - 3000] ]);
		}
	}

	for(let dev of Object.keys(conf["inverters"]))
	{
		if(conf["inverters"][dev]["showdevice"] & conf["inverters"][dev]["meter"])
		{
			coG["Export"+dev]= new sGauge("chart_Export"+dev,"S",0,5000,['0','1','2','3','4','5kW'],4,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], [conf.txt.gridexport, 0 - 5000] ]);
			coG["Last"+dev]= new sGauge("chart_Last"+dev,"S",0,10000,['0','1','2','3','4','5','6','7','8','9','10kW'],2,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], [conf.txt.load, 0 - 5000] ]);
			coG["Bezug"+dev]= new sGauge("chart_Bezug"+dev,"S",0,10000,['0','1','2','3','4','5','6','7','8','9','10kW'],2,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], [conf.txt.fromgrid, 0 - 10000] ]);
		}
	}

	for(let dev of Object.keys(conf["generators"]))
	{
		if(conf["generators"][dev]["showdevice"])
		{
			max=(Math.ceil((conf["generators"][dev]["maxpower"] ? conf["generators"][dev]["maxpower"] : 10000) / 1000) + 0 ) * 1000;

			coG["PV"+dev]= new sGauge("chart_PV"+dev,"S",0,max,( max > 2000 ? tickerarray(max/1000,1,0," "):tickerarray(max/100,1,0," ") ),4,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], ['PV', 0 - max] ]);
			coG["PV1"+dev]= new sGauge("chart_PV1"+dev,"S",0,4000,['0','1','2','3','4kW'],4,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], ['PV1', 0 - 4000] ]);
			coG["PV2"+dev]= new sGauge("chart_PV2"+dev,"S",0,4000,['0','1','2','3','4kW'],4,{ suffix: 'W', fractionDigits: 0 },[ ['Label', 'Value'], ['PV2', 0 - 4000] ]);
		}
	}
}

function setPVdata()
{

	AreaChart=true;

	for(let dev of Object.keys(conf["batteries"]))
	{
		if(get_el('linechart_bat'+dev))
		{
			coG["soc"+dev] = new GLOptions('linechart_bat'+dev);
			coG["soc"+dev].options.chartArea.height="84%";
			coG["soc"+dev].options.legend={position: 'none'};
			coG["soc"+dev].options.vAxis.viewWindow={ min:0, max:100 };
			coG["soc"+dev].options.vAxis.ticks=null;
			coG["soc"+dev].options.vAxis.ticks=tickerarray(100,25);
			coG["soc"+dev].options.colors=['red'];
			coG["soc"+dev].options.colors=[conf.color.batchart1,conf.color.batchart2,conf.color.batchart3];
			coG["soc"+dev].options.vAxis.title='SOC';
			coG["soc"+dev].options.lineWidth=1;
		}
	}

	for(let dev of Object.keys(conf["inverters"]))
	{
		if(get_el('linechart_grid'+dev))
		{
			coG["grid"+dev] = new GLOptions('linechart_grid'+dev,AreaChart);
			coG["grid"+dev].options.chartArea.height="84%";
			coG["grid"+dev].options.vAxis.minValue=0;
			coG["grid"+dev].options.colors=[conf.color.last,conf.color.bezug,conf.color.einspeisung];
			coG["grid"+dev].options.lineWidth=1;

			if(conf["inverters"][dev].gridmeter)
			{
				coG["ugrid"+dev] = new GLOptions('linechart_ugrid'+dev);
				coG["ugrid"+dev].options.chartArea.height="54%";
				coG["ugrid"+dev].options.height=108;
				coG["ugrid"+dev].options.vAxis.viewWindow={ min:210, max:250 };
				coG["ugrid"+dev].options.vAxis.ticks=tickerarray(250,25,210);
				coG["ugrid"+dev].options.colors=[conf.color.gridu];
				coG["ugrid"+dev].options.title='Netzspannung';
				coG["ugrid"+dev].options.lineWidth=1;

				coG["fgrid"+dev] = new GLOptions('linechart_fgrid'+dev);
				coG["fgrid"+dev].options.legend={position: 'none'};
				coG["fgrid"+dev].options.chartArea.height="54%";
				coG["fgrid"+dev].options.height=108;
				coG["fgrid"+dev].options.vAxis.viewWindow={ min:48, max:52 };
				coG["fgrid"+dev].options.vAxis.ticks=tickerarray(52,1,48);
				coG["fgrid"+dev].options.colors=[conf.color.gridf];
				coG["fgrid"+dev].options.title='Netzfrequenz';
				coG["fgrid"+dev].options.lineWidth=1;
			}
		}
	}

	for(let dev of Object.keys(conf["generators"]))
	{
		if(get_el('linechart_pv'+dev))
		{
			if(conf["generators"][dev]["maxpower"])
			{
				pvmax=conf["generators"][dev]["maxpower"];
			}else{
				pvmax=1000;
			}
			//ticker=new Array(0);
			//for(let i=2500;i<=pvmax;i+=2500)
			//	ticker[ticker.length]=i;

			coG["pv"+dev] = new GLOptions('linechart_pv'+dev,AreaChart);
			coG["pv"+dev].options.chartArea.height="84%";
			coG["pv"+dev].options.legend={position: 'none'};
			if(pvmax > 2500)
				coG["pv"+dev].options.vAxis.ticks=tickerarray(pvmax);
			else
				coG["pv"+dev].options.vAxis.ticks=tickerarray(pvmax,250);
			coG["pv"+dev].options.vAxis.viewWindow={ min:0, max:pvmax };
			coG["pv"+dev].options.colors=[conf.color.pvall, conf.color.ladung];
			coG["pv"+dev].options.lineWidth=1;
		}
	}

}


function single_grid()
{

	grid_max=0;

	coG["grid"] = new GLOptions('chart_singleG');
	coG["grid"].options.chartArea.height="84%";
	coG["grid"].options.vAxis.minValue=0;
	coG["grid"].options.width=1480;
	coG["grid"].options.height=368;


	coG["ugrid"] = new GLOptions('chart_singleGU');
	coG["ugrid"].options.chartArea.height="54%";
	coG["ugrid"].options.height=108;
	coG["ugrid"].options.vAxis.viewWindow={ min:210, max:250 };
	coG["ugrid"].options.vAxis.ticks=[210, 230, 250];
	coG["ugrid"].options.colors=[conf.color.gridu];
	coG["ugrid"].options.title=conf.txt.gridvoltage;
	coG["ugrid"].options.lineWidth=1;
	coG["ugrid"].options.width=1480;
	coG["ugrid"].options.height=208;

	coG["fgrid"] = new GLOptions('chart_singleGF');
	coG["fgrid"].options.legend={position: 'none'};
	coG["fgrid"].options.chartArea.height="54%";
	coG["fgrid"].options.height=108;
	coG["fgrid"].options.vAxis.viewWindow={ min:40, max:60 };
	coG["fgrid"].options.vAxis.ticks=[40, 50, 60];
	coG["fgrid"].options.colors=[conf.color.gridf];
	coG["fgrid"].options.title=conf.txt.gridfreq;
	coG["fgrid"].options.lineWidth=1;
	coG["fgrid"].options.width=1480;
	coG["fgrid"].options.height=208;


	var aj=$.ajax({
		url:"?mode=pdata&type=singlegrid",
		dataType: 'json',
		async: false}).responseText;
	var R=jQuery.parseJSON(aj);

	if(R.cnt)
	{
		coG["grid"].data=new google.visualization.DataTable();
		coG["grid"].data.addColumn('datetime', conf.txt.datetime);
		coG["grid"].data.addColumn('number', conf.txt.load);
		coG["grid"].data.addColumn('number', conf.txt.fromgrid);
		coG["grid"].data.addColumn('number', conf.txt.togrid);

		coG["ugrid"].data=new google.visualization.DataTable();
		coG["ugrid"].data.addColumn('datetime', conf.txt.datetime);
		coG["ugrid"].data.addColumn('number', conf.txt.voltage);

		coG["fgrid"].data=new google.visualization.DataTable();
		coG["fgrid"].data.addColumn('datetime', conf.txt.datetime);
		coG["fgrid"].data.addColumn('number', conf.txt.freq);
	}

	for (var i = 0; i < R.cnt; i++)
	{
		var dt=new Date(R.data[i].t);
		coG["grid"].data.addRow([ dt, R.data[i].pload, R.data[i].puser, R.data[i].pgrid ]);

		if(R.data[i].pload>grid_max) grid_max=R.data[i].pload;
		if(R.data[i].puser>grid_max) grid_max=R.data[i].puser;
		if(R.data[i].pgrid>grid_max) grid_max=R.data[i].pgrid;

		coG["ugrid"].data.addRow([ dt, R.data[i].ugrid ]);
		coG["fgrid"].data.addRow([ dt, R.data[i].fgrid ]);

		if(R.data[i].ugrid>grid_u.max) grid_u.max=R.data[i].ugrid;
		if(R.data[i].ugrid<grid_u.min) grid_u.min=R.data[i].ugrid;

		if(R.data[i].fgrid>grid_f.max) grid_f.max=R.data[i].fgrid;
		if(R.data[i].fgrid<grid_f.min) grid_f.min=R.data[i].fgrid;
	}

	grid_max=(Math.ceil(grid_max / 1000) + 0 ) * 1000;

	if(grid_max<2000) grid_max=2000;


	coG["ugrid"].options.title=conf.txt.gridvoltage+' (min/max '+grid_u.min+' V / '+grid_u.max+' V)';
	coG["fgrid"].options.title=conf.txt.gridfreq+' (min/max '+grid_f.min+' Hz / '+grid_f.max+' Hz)';


	coG["grid"].options.vAxis.viewWindow={ min:0, max:grid_max };
	coG["grid"].chart.draw(coG["grid"].data, coG["grid"].options);
	coG["ugrid"].chart.draw(coG["ugrid"].data, coG["ugrid"].options);
	coG["fgrid"].chart.draw(coG["fgrid"].data, coG["fgrid"].options);

}

function pvmppt()
{
	this.pv1=0;
	this.pv2=0;
	this.pv3=0;
	this.pv4=0;
	this.ppv=0;
	this.max=0;
}

function single_thermo(von,bis)
{

	AreaChart=false;

	let colors = conf["color"]["thermoall"].split(",");

	var JSON=$.ajax({
		url:"?mode=pdata&type=singlethermo&datvon="+von+"&datbis="+bis,
		dataType: 'json',
		async: false}).responseText;
	var R=jQuery.parseJSON(JSON);

//console.log("Query", R.cnt);
//console.log("Stuff", R);

	for(let dev of Object.keys(conf["thermo"]))
	{

		console.log("EL", "thermo"+dev);

		idx="thermo"+dev;

//console.log("dev "+dev,R.devices[dev].cnt);

		coG[idx] = new GLOptions('chart_thermo'+dev,AreaChart);
		coG[idx].options.chartArea.height="84%";
		coG[idx].options.vAxis.minValue=0;
		coG[idx].options.width=1480;
		coG[idx].options.height=328;
		coG[idx].options.curveType="";

		if(R.cnt)
		{
			coG[idx].data=new google.visualization.DataTable();

			let col = [];

			coG[idx].data.addColumn('datetime', conf.txt.datetime);

			for(i=1;i<=R.devices[dev].cnt;i++)
			{
				coG[idx].data.addColumn('number', R.devices[dev]["alias"][i - 1]);
				col[col.length]=colors[i];
			}
			coG[idx].options.colors=col;

		}

	}


	for (var i = 0; i < R.cnt; i++)
	{
		idx="thermo"+R.data[i].device;


		if(coG[idx])
		{
			var dt=new Date(R.data[i].werte[0]);

			//console.log("DATA",R.data[i]);

			R.data[i].werte[0]=dt;

//console.log("eldev "+idx,R.data[i].werte);


			coG[idx].data.addRow(R.data[i].werte);
		}
	}



	for(let dev of Object.keys(conf["thermo"]))
	{
		idx="thermo"+dev;
		coG[idx].options.vAxis.ticks=tickerarray(100,10);
		coG[idx].options.vAxis.viewWindow={ min:0, max:100 };

		coG[idx].chart.draw(coG[idx].data, coG[idx].options);
	}


}

function single_pv(von,bis)
{

	pvmax=Array();

	AreaChart=true;

	var JSON=$.ajax({
		url:"?mode=pdata&type=singlepv&datvon="+von+"&datbis="+bis,
		dataType: 'json',
		async: false}).responseText;
	var R=jQuery.parseJSON(JSON);

//console.log("Query", R.cnt);



	for(let dev of Object.keys(conf["generators"]))
	{
		coG["pv"+dev] = new GLOptions('chart_singleP'+dev,AreaChart);
		coG["pv"+dev].options.chartArea.height="84%";
		coG["pv"+dev].options.vAxis.minValue=0;
		coG["pv"+dev].options.width=1480;
		coG["pv"+dev].options.height=328;

		coG["pv1"+dev] = new GLOptions('chart_singleP1'+dev,AreaChart);
		coG["pv1"+dev].options.chartArea.height="84%";
		coG["pv1"+dev].options.vAxis.minValue=0;
		coG["pv1"+dev].options.width=1480;
		coG["pv1"+dev].options.height=328;

		if(R.cnt)
		{
			coG["pv"+dev].data=new google.visualization.DataTable();
			coG["pv"+dev].data.addColumn('datetime', conf.txt.datetime);
			coG["pv"+dev].data.addColumn('number', conf.txt.totalgc);
			coG["pv"+dev].data.addColumn('number', conf.txt.batcharge);


			coG["pv1"+dev].data=new google.visualization.DataTable();
			coG["pv1"+dev].data.addColumn('datetime', conf.txt.datetime);
			for(i=1;i<=conf.generators[dev]["mppts"];i++)
				coG["pv1"+dev].data.addColumn('number', 'MPPT'+i);
		}
		pvmax[dev]=new pvmppt
	}

	for (var i = 0; i < R.cnt; i++)
	{
		var dt=new Date(R.data[i].t);
		//console.log("DATA",R.data[i]);
		if(coG["pv"+R.data[i].pdevice])
		{
			coG["pv"+R.data[i].pdevice].data.addRow([ dt, R.data[i].ppv, R.data[i].pcharge ]);
			if(R.data[i].ppv > pvmax[R.data[i].pdevice].max) pvmax[R.data[i].pdevice].max=R.data[i].ppv;

			coG["pv1"+R.data[i].pdevice].data.addRow( uarr(conf.generators[R.data[i].pdevice]["mppts"]+1, dt, R.data[i].ppv1, R.data[i].ppv2, R.data[i].ppv3, R.data[i].ppv4) );

			if(R.data[i].ppv1>pvmax[R.data[i].pdevice].pv1) pvmax[R.data[i].pdevice].pv1=R.data[i].ppv1;
			if(R.data[i].ppv2>pvmax[R.data[i].pdevice].pv2) pvmax[R.data[i].pdevice].pv2=R.data[i].ppv2;
			if(R.data[i].ppv3>pvmax[R.data[i].pdevice].pv3) pvmax[R.data[i].pdevice].pv3=R.data[i].ppv3;
			if(R.data[i].ppv4>pvmax[R.data[i].pdevice].pv4) pvmax[R.data[i].pdevice].pv4=R.data[i].ppv4;
		}
	}

	for(let dev of Object.keys(conf["generators"]))
	{
		pvmax[dev].pv1=(Math.ceil(pvmax[dev].pv1 / 1000) + 0 ) * 1000;
		pvmax[dev].pv2=(Math.ceil(pvmax[dev].pv2 / 1000) + 0 ) * 1000;
		pvmax[dev].pv3=(Math.ceil(pvmax[dev].pv3 / 1000) + 0 ) * 1000;
		pvmax[dev].pv4=(Math.ceil(pvmax[dev].pv4 / 1000) + 0 ) * 1000;

		if(conf.generators[dev]["mppts"]>1 & pvmax[dev].pv2>pvmax[dev].pv1) pvmax[dev].pv1=pvmax[dev].pv2;
		if(conf.generators[dev]["mppts"]>2 & pvmax[dev].pv3>pvmax[dev].pv1) pvmax[dev].pv1=pvmax[dev].pv3;
		if(conf.generators[dev]["mppts"]>3 & pvmax[dev].pv4>pvmax[dev].pv1) pvmax[dev].pv1=pvmax[dev].pv4;

		let pmax=Math.ceil(pvmax[dev].max / 1000) * 1000;
		if( pvmax[dev].max > 5000 )
			tick=1000;
		else
			if( pvmax[dev].max > 2500 )
				tick=500;
			else
				tick=100;
		coG["pv"+dev].options.vAxis.ticks=tickerarray(pmax,tick);
		coG["pv"+dev].options.vAxis.viewWindow={ min:0, max:pmax };
		coG["pv"+dev].options.colors=[conf["color"]["pvall"],conf["color"]["gridu"]];


		pmax=Math.ceil(pvmax[dev].pv1 / 1000) * 1000;

		coG["pv1"+dev].options.vAxis.ticks=tickerarray(pmax,tick);
		coG["pv1"+dev].options.vAxis.viewWindow={ min:0, max:pvmax[dev].pv1 };
		coG["pv1"+dev].options.colors=[conf.color.pv1, conf.color.pv2, conf.color.pv3, conf.color.pv4];

		coG["pv"+dev].chart.draw(coG["pv"+dev].data, coG["pv"+dev].options);
		coG["pv1"+dev].chart.draw(coG["pv1"+dev].data, coG["pv1"+dev].options);
	}

}

function readPVdata()
{

	grid_max=Array();
	grid_u=Array();
	grid_f=Array();

	var prec=get_el("precis").checked ? "&prec=1":"";
	var JSON=$.ajax( {url:"?mode=pdata&type=charts"+prec,dataType: 'json',async: false} ).responseText;
	var R=jQuery.parseJSON(JSON);

	//console.log("readPVdata()");
	//console.log("?mode=pdata&type=charts"+prec, "Cnt: "+R.cnt);

	if(R.cnt)
	{
		for(let dev of Object.keys(conf["generators"]))
		{
			if(conf["generators"][dev]["showdevice"])
			{
				coG["pv"+dev].data=new google.visualization.DataTable();
				coG["pv"+dev].data.addColumn('datetime', conf.txt.datetime);
				coG["pv"+dev].data.addColumn('number', 'PV');
				coG["pv"+dev].data.addColumn('number', 'Ladung');
			}
		}

		for(let dev of Object.keys(conf["batteries"]))
		{
			if(conf["batteries"][dev]["showdevice"])
			{
				coG["soc"+dev].data=new google.visualization.DataTable();
				coG["soc"+dev].data.addColumn('datetime', conf.txt.datetime);
				coG["soc"+dev].data.addColumn('number', conf.txt.SOC);
				coG["soc"+dev].data.addColumn('number', 'Aufladung % von 3KW');
				coG["soc"+dev].data.addColumn('number', 'Entladung % von 3KW');
			}
		}

		for(let dev of Object.keys(conf["inverters"]))
		{
			if(conf["inverters"][dev]["showdevice"])
			{
				coG["grid"+dev].data=new google.visualization.DataTable();
				coG["grid"+dev].data.addColumn('datetime', conf.txt.datetime);
				coG["grid"+dev].data.addColumn('number', conf.txt.load);
				coG["grid"+dev].data.addColumn('number', conf.txt.fromgrid);
				coG["grid"+dev].data.addColumn('number', conf.txt.togrid);

				coG["ugrid"+dev].data=new google.visualization.DataTable();
				coG["ugrid"+dev].data.addColumn('datetime', conf.txt.datetime);
				coG["ugrid"+dev].data.addColumn('number', conf.txt.voltage);

				coG["fgrid"+dev].data=new google.visualization.DataTable();
				coG["fgrid"+dev].data.addColumn('datetime', conf.txt.datetime);
				coG["fgrid"+dev].data.addColumn('number', conf.txt.freq);

				grid_u[dev]=Array();grid_u[dev].min=10000;grid_u[dev].max=-1;
				grid_f[dev]=Array();grid_f[dev].min=10000;grid_f[dev].max=-1;
				grid_max[dev]=0;
			}
		}
	}

	for (var i = 0; i < R.cnt; i++)
	{
		var dt=new Date(R.data[i].t);
		var dev=R.data[i].device;

		if(conf["batteries"][dev])
		{
			if(conf["batteries"][dev]["showdevice"])
			{
				coG["soc"+dev].data.addRow([ dt, R.data[i].soc, R.data[i].pcharge / 30, R.data[i].pdischarge / 30]);
			}
		}

		if(conf["generators"][dev])
		{
			if(conf["generators"][dev]["showdevice"])
			{
				coG["pv"+dev].data.addRow([ dt, R.data[i].ppv, R.data[i].pcharge ]);
			}
		}

		if(conf["inverters"][dev])
		{
			if(conf["inverters"][dev]["showdevice"] & R.data[i].ugrid != -1 & R.data[i].fgrid != -1)
			{
				coG["ugrid"+dev].data.addRow([ dt, R.data[i].ugrid ]);
				coG["fgrid"+dev].data.addRow([ dt, R.data[i].fgrid ]);
				coG["grid"+dev].data.addRow([ dt, R.data[i].pload, R.data[i].puser, R.data[i].pgrid ]);

				if(R.data[i].ugrid>grid_u[dev].max) grid_u[dev].max=R.data[i].ugrid;
				if(R.data[i].ugrid<grid_u[dev].min) grid_u[dev].min=R.data[i].ugrid;

				if(R.data[i].fgrid>grid_f[dev].max) grid_f[dev].max=R.data[i].fgrid;
				if(R.data[i].fgrid<grid_f[dev].min) grid_f[dev].min=R.data[i].fgrid;

				if(R.data[i].pload>grid_max[dev]) grid_max[dev]=R.data[i].pload;
				if(R.data[i].puser>grid_max[dev]) grid_max[dev]=R.data[i].puser;
				if(R.data[i].pgrid>grid_max[dev]) grid_max[dev]=R.data[i].pgrid;
			}
		}
	}

	for(let dev of Object.keys(conf["inverters"]))
	{

		if(conf["inverters"][dev]["showdevice"])
		{
			coG["ugrid"+dev].options.title=conf.txt.gridvoltage+' (min/max '+grid_u[dev].min+' V / '+grid_u[dev].max+' V)';
			coG["fgrid"+dev].options.title=conf.txt.gridfreq+' (min/max '+grid_f[dev].min+' Hz / '+grid_f[dev].max+' Hz)';

			grid_max[dev]=(Math.ceil(grid_max[dev] / 1000) + 0 ) * 1000;
			if(grid_max[dev]<2000) grid_max[dev]=2000;
			coG["grid"+dev].options.vAxis.viewWindow={ min:0, max:grid_max[dev] };
		}
	}
	//console.log("readPVdata()-ENDE");
}

function drawChartData()
{
	readPVdata();

	for(let dev of Object.keys(conf["batteries"]))
	{
		if(conf["batteries"][dev]["showdevice"])
		{
			coG["soc"+dev].draw();
		}
	}

	for(let dev of Object.keys(conf["generators"]))
	{
		if(conf["generators"][dev]["showdevice"])
		{
			coG["pv"+dev].draw();
		}
	}

	for(let dev of Object.keys(conf["inverters"]))
	{
		if(conf["inverters"][dev]["showdevice"])
		{
			coG["grid"+dev].draw();
			coG["fgrid"+dev].draw();
			coG["ugrid"+dev].draw();
		}
	}
}

function submit_set_dates()
{
	if(get_el("datvon"))	set_val("datvon",get_val("vjahr")+"-"+get_val("vmonat")+"-"+get_val("vtag"));
	if(get_el("datbis"))	set_val("datbis",get_val("bjahr")+"-"+get_val("bmonat")+"-"+get_val("btag"));
}