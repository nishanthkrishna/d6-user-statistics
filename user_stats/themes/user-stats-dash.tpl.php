<script>
    google.load('visualization', '1', {packages:['corechart', 'table']});
    var user_data=<?php print $data; ?>;
 
    jQuery(document).ready(function(){
        var data = new google.visualization.DataTable();
        data.addColumn('date', 'Date');
        data.addColumn('number', 'Registrations');
        data.addRows(user_data.length);
          
        var counter = 0;
        for(var i in user_data) {
           var item = user_data[i];
            var date_split = item['date'].split('-');
            var date_object = new Date(
            parseInt(date_split[0], 10),
            parseInt(date_split[1], 10) - 1,
            parseInt(date_split[2], 10)
        );
            data.setValue(counter, 0, date_object);
            data.setValue(counter, 1, parseInt(item['count'], 10));
            counter++;
        }

        var chart = new google.visualization.LineChart(document.getElementById('registration_statistics_table_line_chart'));
        chart.draw(data, {width: '750', height: 400, legend: 'none'});

        var table = new google.visualization.Table(document.getElementById('registration_statistics_table'));
        table.draw(data, {width: 750, sortColumn: 0, page: 'enable', pageSize: 10});
    });


</script>
<div id="registration_statistics_table_line_chart" style="float:left"></div>
<div id="registration_statistics_table" style="float:left"></div>
