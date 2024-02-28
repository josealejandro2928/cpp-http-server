const express = require('express');
const app = express();
const port = 3000;

app.get('/', (req, res) => {
    res.send('Hello World!');
})

app.get('/hello', (req, res) => {
    console.log("New connection")
    let sum = 0;
    for (let i = 0; i < 1000000000; i++) {
        sum += i;
    }
    res.status(200).send(`The computation was: ${sum}`);
})
app.listen(port, () => {
    console.log(`Node Server is running on port ${port}`);
});
