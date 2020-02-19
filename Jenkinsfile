node {
    try {
        checkout scm
        stage('Preparation') {
            sh 'mv test_suite/project.yml test_suite/project.yml.back'
            sh 'ceedling new test_suite' // prepare test_suite insiders
            sh 'mv test_suite/project.yml.back test_suite/project.yml'
            sh 'ceedling upgrade test_suite' // upgrade ceedling
        }
        
        stage('Unit Test') {
            dir("test_suite") {
                sh 'ceedling gcov:all'
                sh 'ceedling utils:gcov'
                sh 'cat build/artifacts/gcov/GcovCoverageResults.xml'
            }
        }
    } catch (e) {
        throw e
    } finally {
        stage('Generating Coverage') {
            cobertura coberturaReportFile: 'test_suite/build/artifacts/gcov/GcovCoverageResults.xml'
        }
    }
}
