import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    height: 30
    color: "#dddddd"
    Row {
        Text {
            text: "Cassandra Status: " + databaseManager.cassandraStatus
        }
        Text {
            text: "Redis Status: " + databaseManager.redisStatus
        }
    }
}
