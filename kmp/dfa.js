function make_dfa(pattern) {
    const patternLen = pattern.length
    let dp = [];
    for (let i = 0; i < patternLen; i++) {
        dp[i] = [];
        for (let j = 0; j < 256; j++)
            dp[i][j] = 0;
    }
    dp[0][pattern.charCodeAt(0)] = 1
    let x = 0;

    for (let j = 1; j < patternLen; j++) {
        for (let c = 0; c < 256; c++) {
            if (pattern.charCodeAt(j) === c) {
                dp[j][c] = j + 1;
            }
            else {
                dp[j][c] = dp[x][c]
            }
        }
        x = dp[x][pattern.charCodeAt(j)]; // 查找已有状态信息里面的状态
    }
    return dp
}

function search(str, pattern, dp) {
    let j = 0
    for (let i = 0; i < str.length; i++) {
        j = dp[j][str.charCodeAt(i)];
        if (j == pattern.length) return i - j + 1;
    }
    return -1;
}


let pattern = 'ababc'
const dp = make_dfa(pattern)

const str = 'ababdababc'
const index = search(str, pattern, dp)
console.log(index)
