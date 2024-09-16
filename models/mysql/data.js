import mysql from 'mysql2/promise';
import credentials from '../../utils/credentials.js';

const config = {
  host: credentials.host,
  user: credentials.user,
  port: credentials.port,
  password: credentials.password,
  database: credentials.database
};

export class DataModel {
  static async register ({ data }) {
    let connection;
    try {
      connection = await mysql.createConnection(config);
      const { ppm, date, time } = data;
      if (!ppm || !date || !time) return '{ er: there are null values }';
      const query = 'INSERT INTO registros(ppm, fecha, tiempo) VALUES (?, ?, ?);';
      const result = connection.query(query, [ppm, date, time]);
      return result;
    } catch (e) {
      console.error('Error inserting the data', e);
      return '{ er: an error occured during the insertion of the data }';
    } finally {
      if (connection) await connection.end();
    }
  }
}
