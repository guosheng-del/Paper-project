package com.example.aliyunapplication;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class JDBCUtil {

    private static JDBCUtil jdbcUtil;
    private Connection connection;

    private JDBCUtil() {

    }

    public static JDBCUtil getInstance() {
        if(jdbcUtil == null) {
            jdbcUtil = new JDBCUtil();
        }
        return jdbcUtil;
    }

    public Connection getConnection() throws Exception {
        if(connection == null) {
            //1、装载驱动
            Class.forName("com.mysql.jdbc.Driver");
            //2.定义连接url   阿里云数据库
            String url="jdbc:mysql://rm-uf6j0wh0q75jc4v0gho.mysql.rds.aliyuncs.com:3306/stm32_dpj076?serverTimezone=GMT%2B8&useUnicode=true&characterEncoding=UTF-8&useSSL=false";
            //3.定义账号和密码
            String username="stm32_dpj076";
            String password="stm32_dpj076";
            //4.获得连接对象
            connection = DriverManager.getConnection(url, username, password);
        }
        return connection;
    }


    public List<String[]> getPetLogData() {
        List<String[]> data = new ArrayList<>();
        PreparedStatement statement = null;
        ResultSet resultSet = null;
        try {
            Connection connection = getConnection();
            String sql = "SELECT * FROM logs";
            statement = connection.prepareStatement(sql);
            resultSet = statement.executeQuery();

            Map<String, String> provinceMap = new HashMap<>();
            provinceMap.put("5180", "冀");
            provinceMap.put("664b", "晋");
            provinceMap.put("8fbd", "辽");
            provinceMap.put("5409", "吉");
            provinceMap.put("9ed1", "黑");
            provinceMap.put("82cf", "苏");
            provinceMap.put("6d59", "浙");
            provinceMap.put("7696", "皖");
            provinceMap.put("95fd", "闽");
            provinceMap.put("8d63", "赣");
            provinceMap.put("9c81", "鲁");
            provinceMap.put("8c6b", "豫");
            provinceMap.put("9102", "鄂");
            provinceMap.put("6e58", "湘");
            provinceMap.put("7ca4", "粤");
            provinceMap.put("743c", "琼");
            provinceMap.put("5ddd", "川");
            provinceMap.put("8d35", "贵");
            provinceMap.put("4e91", "云");
            provinceMap.put("9655", "陕");
            provinceMap.put("7518", "甘");
            provinceMap.put("9752", "青");
            provinceMap.put("8499", "蒙");
            provinceMap.put("6842", "桂");
            provinceMap.put("5b81", "宁");
            provinceMap.put("4eac", "京");

            while (resultSet.next()) {
                String font = resultSet.getString("font");
                String number = resultSet.getString("number");
                String flag = resultSet.getString("flag");
                String hour = resultSet.getString("hour");
                String minute= resultSet.getString("minute");
                String sec = resultSet.getString("sec");
                String money= resultSet.getString("money");
                String time = resultSet.getString("time");

                String stoptime;
                if(flag.equals("1")){
                     stoptime = "0";
                }else{
                    stoptime= hour+":"+minute+":"+sec;
                }

                String FlagStatus ;
                if(flag.equals("1")){
                    FlagStatus = "进场";
                }else{
                    FlagStatus= "出场";
                }
                String sourceProvince = provinceMap.get(font);

                String[] rowData = {time,sourceProvince,number, FlagStatus,stoptime,money};
                data.add(rowData);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (resultSet != null) {
                    resultSet.close();
                }
                if (statement != null) {
                    statement.close();
                }
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return data;
    }


    public List<String[]> getPetLogData1(String provinceAbbreviation, String num) {
        List<String[]> data = new ArrayList<>();
        PreparedStatement statement = null;
        ResultSet resultSet = null;
        try {
            Connection connection = getConnection();

            // 省份映射
            Map<String, String> provinceMap = new HashMap<>();
            provinceMap.put("5180", "冀");
            provinceMap.put("664b", "晋");
            provinceMap.put("8fbd", "辽");
            provinceMap.put("5409", "吉");
            provinceMap.put("9ed1", "黑");
            provinceMap.put("82cf", "苏");
            provinceMap.put("6d59", "浙");
            provinceMap.put("7696", "皖");
            provinceMap.put("95fd", "闽");
            provinceMap.put("8d63", "赣");
            provinceMap.put("9c81", "鲁");
            provinceMap.put("8c6b", "豫");
            provinceMap.put("9102", "鄂");
            provinceMap.put("6e58", "湘");
            provinceMap.put("7ca4", "粤");
            provinceMap.put("743c", "琼");
            provinceMap.put("5ddd", "川");
            provinceMap.put("8d35", "贵");
            provinceMap.put("4e91", "云");
            provinceMap.put("9655", "陕");
            provinceMap.put("7518", "甘");
            provinceMap.put("9752", "青");
            provinceMap.put("8499", "蒙");
            provinceMap.put("6842", "桂");
            provinceMap.put("5b81", "宁");
            provinceMap.put("4eac", "京");

            // 根据省份简称获取对应的 font 值
            String font = null;
            for (Map.Entry<String, String> entry : provinceMap.entrySet()) {
                if (entry.getValue().equals(provinceAbbreviation)) {
                    font = entry.getKey(); // 获取对应的 font 值
                    break;
                }
            }

            // 使用参数化查询，避免SQL注入
            String sql = "SELECT * FROM logs WHERE number = ?";
            if (font != null) {
                sql += " OR font = ?";
            }

            statement = connection.prepareStatement(sql);
            statement.setString(1, num.trim()); // 设置车牌号，去除前后空格

            if (font != null) {
                statement.setString(2, font); // 设置省份对应的 font 值
            }

            System.out.println("执行的SQL: " + statement.toString()); // 打印执行的SQL语句以供调试
            resultSet = statement.executeQuery();

            // 打印车牌号和font用于调试
            System.out.println("查询的车牌号: " + num.trim() + ", 对应的font: " + font);

            while (resultSet.next()) {
                String number = resultSet.getString("number");
                String flag = resultSet.getString("flag");
                String hour = resultSet.getString("hour");
                String minute = resultSet.getString("minute");
                String sec = resultSet.getString("sec");
                String money = resultSet.getString("money");
                String time = resultSet.getString("time");

                String stoptime;
                if (flag.equals("1")) {
                    stoptime = "0";
                } else {
                    stoptime = hour + ":" + minute + ":" + sec;
                }

                String FlagStatus;
                if (flag.equals("1")) {
                    FlagStatus = "进场";
                } else {
                    FlagStatus = "出场";
                }

                String[] rowData = {time, provinceAbbreviation, number, FlagStatus, stoptime, money};
                data.add(rowData);
            }

            if (data.isEmpty()) {
                System.out.println("未找到匹配的记录: font = " + font + ", number = " + num.trim());
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (resultSet != null) {
                    resultSet.close();
                }
                if (statement != null) {
                    statement.close();
                }
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return data;
    }



}
