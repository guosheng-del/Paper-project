package com.example.aliyunapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;


public class MenuActivity extends AppCompatActivity {
    private Button btnRealtimeData;
    private Button btnHistoryData;
    private Button btnDeviceControl;
    private Button btnThresholdSetting;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.menu);

        // 初始化按钮
        btnRealtimeData = findViewById(R.id.btn_realtime_data);
        btnHistoryData = findViewById(R.id.btn_history_data);


        // 设置点击事件处理逻辑
        btnRealtimeData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 跳转到实时监测页面
                Intent intent = new Intent(MenuActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });

        btnHistoryData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MenuActivity.this, HistoryActivity.class);
                startActivity(intent);
            }
        });

//
    }

}
