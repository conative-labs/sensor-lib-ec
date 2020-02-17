node {
    checkout scm
    stage('Preparation') {
        sh 'mv test_suite/project.yml test_suite/project.yml.back'
        sh 'ceedling new test_suite' // prepare test_suite insiders
        sh 'mv test_suite/project.yml.back test_suite/project.yml'
        sh 'ceedling upgrade test_suite' // upgrade ceedling
    }
    
    stage('Unit Test') {
        dir("test_suite") {
            sh 'ceedling' // run all tests
        }
    }
}
