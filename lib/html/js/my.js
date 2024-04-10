
function get_text(el)
{
	return get_el(el).innerHTML;
}

function set_text(el,text)
{
	get_el(el).innerHTML=text;
}

function set_val(el,val)
{
	get_el(el).value=val;
}

function get_val(el)
{
	var ret="";
	if(x=get_el(el))
	{
		ret=_get_val(x)
	}
	return ret;
}

function _get_val(el)
{
	var ret="";
	if(el)
	{
		switch(el.type)
		{
		//MyMsg(el.name+"="+el.type);
		case "checkbox":
		case "radio":
			if(el.checked)
			{
				if(el.value)
					ret=el.value;
				else
					ret="on";
			}
			break;
		case "text":
		case "hidden":
		case "textarea":
		case "password":
		case "select-one":
			ret=el.value;
			break;
		case "select-multiple":
			var list="";
			for(var s=el.length-1;s>=0;s--)
			{
				if(el.options[s].selected)
				{
					if(list.length) list=list+"|";
					list=list+el.options[s].value;
				}
			}
			//MyMsg(el.name+"="+list);
			ret=list;
			break;
		default:
			//MyMsg("EL "+el.name+" ("+el.id+") = "+el.type+" : "+el.form);
			if(el.length)	//form?  && get_el(el.elements[0])
			{
				var list="";
				for (var i=0;i<el.length;i++)
				{
					if(el.elements[i].type=="checkbox")
					{
						if(el.elements[i].checked)
						{
							var t="";
							var y=get_el(el.elements[i].id+"_text");
							if(y)
							{
								t="~"+y.value;
							}
							if(list.length) list=list+"|";
							list=list+el.elements[i].value+t;
						}
					}
				}
				//MyMsg(el.name+"="+list);
				ret=list;
			}else{
				ret=el.value;
			}
			break;
		}
	}
	return ret;
}

function get_el(el)
{
	if (document.getElementById(el))
	{
		return document.getElementById(el);
	}else{
		//alert("Das Element "+el+" wurde nicht gefunden!");
		return false;
	}
}

function get_openerel(el)
{
	if (window.opener.document.getElementById(el))
	{
		return window.opener.document.getElementById(el);
	}else{
		return false;
	}
}

function myBargraph(id,wert,markercolor="red",bgcolor="#FDFDFD",text="",textk="")
{
	this.id=id;
	this.wert=wert;
	this.bgcolor=bgcolor;
	this.markercolor=markercolor;

	this.ntext="&nbsp;";
	this.ktext=" ";
	if(text!="")
		this.ntext=text;
	if(textk!="")
		this.ntext=this.ntext+" <i>"+textk+"</i>";

	this.draw=function(){
		if(this.wert>99) this.wert=99;
		get_el(this.id).innerHTML="<div style='border:1px solid #747474;width:100%;height:20px;padding-left:1px;padding-right:1px;position:relative;"+
			"background-image: url(?mode=file&fn=pics/hmark100.png);background-repeat:no-repeat;background-size:100% 6px;background-position:left bottom;background-color:"+this.bgcolor+";"+
			"text-align:center;vertical-align:top;color:#000000;font-size:0.9em;font-weight:500;'>"+
			"<div style='opacity:0.65;filter:alpha(opacity=65);width:"+this.wert+"%;background-color:"+this.markercolor+";position:absolute;bottom:1px;height:7px'> </div>"+
			this.ntext+"</div>";
	}
}


// new sGauge("chart_Bat","S|L",0,100,['0','20','40','60','80', '100'],4,{ suffix: '%', fractionDigits: 0 },[ ['Label', 'Value'], ['Batterie', 1] ])
function sGauge(elid,size,min,max,major,minor,fmt,data)
{
	this.data=	new google.visualization.arrayToDataTable( data );
	this.fmt=	new google.visualization.NumberFormat( fmt );
	var spx=	178;
	if(size=="XS")	spx=98;
	if(size=="S")	spx=128;
	if(size=="M")	spx=153;
	this.options=	new GGObj(spx,min,max,major,minor);
	this.chart=	new google.visualization.Gauge(get_el(elid));
	this.setVal=function(val){
		var pval=parseInt(val);
		this.data.setValue(0, 1, pval);
//		console.log("Gauge-val",pval);
		this.fmt.format(this.data, 1);
		this.draw();
	}
	this.draw=function(){
		this.chart.draw(this.data, this.options);
	}
}

function GLOptions(elid,area=false)
{
	this.options={};
	this.options.animation={ startup: true, duration: 800, easing: "inAndOut" };
	this.options.legend={position: 'top', textStyle: {color: 'black', fontStyle: 'normal', fontSize: 12}};
	this.options.chartArea={'width': '80%', 'height': '80%'};
	this.options.colors=['#0099cc','#b30000','green'];
	this.options.backgroundColor=conf.color.LChartBGColor;
	this.options.lineWidth=1;
	this.options.curveType='function';
	this.options.smoothline=true;
	this.options.axes={ y: { "all": { } }, x: { 0: {"side": 'top'} } };
	this.options.vAxis={ "title": '',
		minValue: 0,
		ticks: [0, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000],
		viewWindow: { min:0, max:10000 }
		};
	this.options.width=Lwidth;
	this.options.height=Lheight;
	this.options.title="";
	this.options.titleTextStyle={ color: '#000000', fontSize: 12, bold: false, italic: false } //fontName: 'Times New Roman',
	this.options.is3D=true;
	if(area)
		this.chart=new google.visualization.AreaChart(get_el(elid));
	else
		this.chart=new google.visualization.LineChart(get_el(elid));
	this.data=new google.visualization.DataTable();
	this.draw=function(){
		this.chart.draw(this.data, this.options);
	}
}

function GGObj(size,min,max,major,minor)
{
	this.width=size;
	this.height=size;
	this.min=min;
	this.max=max;
	this.majorTicks=major;
	this.minorTicks=minor;
}

