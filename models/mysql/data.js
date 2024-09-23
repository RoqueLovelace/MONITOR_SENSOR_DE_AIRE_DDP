import mysql from 'mysql2/promise';
import credentials from '../../utils/credentials.js';

const pool = mysql.createPool({
  host: credentials.host,
  user: credentials.user,
  port: credentials.port,
  password: credentials.password,
  database: credentials.database,
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0
});

export class DataModel {
  static async register ({ data }) {
    try {
      const { ppm, date, time } = data;
      if (!ppm || !date || !time) return '{ er: there are null values }';
      const query = 'INSERT INTO registros(ppm, fecha, tiempo) VALUES (?, ?, ?);';
      const [result] = await pool.query(query, [ppm, date, time]);
      return result;
    } catch (e) {
      console.error('Error inserting the data', e);
      return '{ er: an error occurred during the insertion of the data }';
    }
  }

  static async getAll ({ fecha }) {
    let query = 'SELECT ppm, fecha, tiempo FROM registros ';
    const params = [];
    if (fecha) {
      query += 'WHERE fecha LIKE ? ';
      params.push(fecha);
    }

    query += 'ORDER BY fecha DESC, tiempo DESC;';

    const [data] = await pool.query(query, params);
    return data;
  }
}
