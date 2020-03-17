const chai = require('chai');
const chaiHttp = require('chai-http');
var express = require('express');

var app = require("../bin/www");

app.get('/test', function(req, res) {
  res.status(200).json({ name: 'john' });
});

chai.should()
chai.use(chaiHttp)

describe('Server running', () => {
  it('responds with HTTP 200 if server can start', () => {
    return chai.request(app)
      .get('/')
      .then(res => {
        res.should.have.status(200)
      })
      .catch(err => {
        throw err
      })
  })
})
