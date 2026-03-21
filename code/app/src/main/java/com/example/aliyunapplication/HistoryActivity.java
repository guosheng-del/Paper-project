package com.example.aliyunapplication;

import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import java.util.List;
import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONException;
import org.json.JSONObject;

public class HistoryActivity extends AppCompatActivity {

    private TableLayout tableLayout;

    private EditText provinceEditText;
    private EditText plateNumberEditText;
    private Button queryButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.history);

        tableLayout = findViewById(R.id.table_layout);
        tableLayout = findViewById(R.id.table_layout);
        provinceEditText = findViewById(R.id.highestFontEditText1);
        plateNumberEditText = findViewById(R.id.highestNumberEditText1);
        queryButton = findViewById(R.id.open);

        // 执行异步任务查询petlog表数据
        QueryPetLogTask queryPetLogTask = new QueryPetLogTask();
        queryPetLogTask.execute();


    }
    public void onSendButtonClicked(View view) {
        String province = provinceEditText.getText().toString().trim(); // 获取用户输入的省份
        String plateNumber = plateNumberEditText.getText().toString().trim(); // 获取用户输入的车牌号

        // 执行异步任务查询petlog表数据
        QueryPetLogByCriteriaTask queryPetLogByCriteriaTask = new QueryPetLogByCriteriaTask(province, plateNumber);
        queryPetLogByCriteriaTask.execute();
    }



    private class QueryPetLogByCriteriaTask extends AsyncTask<Void, Void, List<String[]>> {
        private String province;
        private String plateNumber;

        public QueryPetLogByCriteriaTask(String province, String plateNumber) {
            this.province = province;
            this.plateNumber = plateNumber;
        }

        @Override
        protected List<String[]> doInBackground(Void... voids) {
            // 在后台线程中执行基于条件的查询操作
            return JDBCUtil.getInstance().getPetLogData1(province, plateNumber); // 使用省份和车牌号查询
        }

        @Override
        protected void onPostExecute(List<String[]> result) {
            // 在主线程中处理查询结果
            if (result != null) {
                // 清空表格内容
                tableLayout.removeAllViews();

                // 添加表头
                TableRow headerRow = new TableRow(HistoryActivity.this);
                headerRow.addView(createTextView("时间"));
                headerRow.addView(createTextView("省份"));
                headerRow.addView(createTextView("车牌号"));
                headerRow.addView(createTextView("状态"));
                headerRow.addView(createTextView("时长"));
                headerRow.addView(createTextView("费用"));

                tableLayout.addView(headerRow);

                // 添加数据行
                for (String[] rowData : result) {
                    TableRow dataRow = new TableRow(HistoryActivity.this);
                    for (String columnData : rowData) {
                        dataRow.addView(createTextView(columnData));
                    }
                    tableLayout.addView(dataRow);
                }
            }
        }

        private TextView createTextView(String text) {
            TextView textView = new TextView(HistoryActivity.this);
            textView.setLayoutParams(new TableRow.LayoutParams(TableRow.LayoutParams.WRAP_CONTENT,
                    TableRow.LayoutParams.WRAP_CONTENT));
            textView.setPadding(8, 8, 8, 8);
            textView.setText(text);
            return textView;
        }
    }


    private class QueryPetLogTask extends AsyncTask<Void, Void, List<String[]>> {

        @Override
        protected List<String[]> doInBackground(Void... voids) {
            // 在后台线程中执行查询操作
            return JDBCUtil.getInstance().getPetLogData();
        }

        @Override
        protected void onPostExecute(List<String[]> result) {
            // 在主线程中处理查询结果
            if (result != null) {
                // 清空表格内容
                tableLayout.removeAllViews();

                // 添加表头
                TableRow headerRow = new TableRow(HistoryActivity.this);
                headerRow.addView(createTextView("时间"));
                headerRow.addView(createTextView("省份"));
                headerRow.addView(createTextView("车牌号"));
                headerRow.addView(createTextView("状态"));
                headerRow.addView(createTextView("时长"));
                headerRow.addView(createTextView("费用"));

                tableLayout.addView(headerRow);

                // 添加数据行
                for (String[] rowData : result) {
                    TableRow dataRow = new TableRow(HistoryActivity.this);
                    for (String columnData : rowData) {
                        dataRow.addView(createTextView(columnData));
                    }
                    tableLayout.addView(dataRow);
                }
            }
        }

        private TextView createTextView(String text) {
            TextView textView = new TextView(HistoryActivity.this);
            textView.setLayoutParams(new TableRow.LayoutParams(TableRow.LayoutParams.WRAP_CONTENT,
                    TableRow.LayoutParams.WRAP_CONTENT));
            textView.setPadding(8, 8, 8, 8);
            textView.setText(text);
            return textView;
        }
    }
}

